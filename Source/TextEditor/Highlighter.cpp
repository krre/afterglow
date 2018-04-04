#include "Highlighter.h"
#include "SyntaxHighlightManager.h"
#include <QtCore>

Highlighter::Highlighter(const QString& fileExt, QTextDocument* parent) : QSyntaxHighlighter(parent) {
    loadRules(fileExt);
}

bool Highlighter::hasExtension(const QString& ext) {
    return SyntaxHighlightManager::getInstance()->hasExtension(ext);
}

void Highlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(static_cast<int>(BlockState::MultilineCommentBegin));

    int startIndex = 0;
    if (static_cast<BlockState>(previousBlockState()) != BlockState::MultilineCommentEnd) {
        startIndex = text.indexOf(commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(static_cast<int>(BlockState::MultilineCommentEnd));
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

void Highlighter::loadRules(const QString& fileExt) {
    if (!SyntaxHighlightManager::getInstance()->hasExtension(fileExt)) return;

    QJsonObject obj = SyntaxHighlightManager::getInstance()->getSyntaxJson(fileExt);

    lang = obj["lang"].toObject()["name"].toString();
    langExt = obj["lang"].toObject()["extension"].toString();

    QJsonObject highighting = obj["highlighting"].toObject();

    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    for (const auto& keyword: highighting["keywords"].toArray()) {
        rule.pattern = QRegularExpression(QString("\\b%1\\b").arg(keyword.toString()));
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QTextCharFormat classFormat;
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");

    valid = true;
}
