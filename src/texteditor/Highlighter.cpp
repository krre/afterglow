#include "Highlighter.h"
#include "SyntaxHighlightManager.h"
#include <QtCore>

Highlighter::Highlighter(const QString& fileExt, QTextDocument* parent) : QSyntaxHighlighter(parent) {
    loadRules(fileExt);
}

bool Highlighter::hasExtension(const QString& ext) {
    return SyntaxHighlightManager::instance()->hasExtension(ext);
}

void Highlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : qAsConst(highlightingRules)) {
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
    if (!SyntaxHighlightManager::instance()->hasExtension(fileExt)) return;

    QJsonObject obj = SyntaxHighlightManager::instance()->getSyntaxJson(fileExt);

    QJsonObject lang = obj["lang"].toObject();
    QJsonObject words = obj["words"].toObject();
    QJsonArray rules = obj["rules"].toArray();
    QJsonArray blocks = obj["blocks"].toArray();
    QJsonObject formats = obj["formats"].toObject();

    langName = lang["name"].toString();
    langExt = lang["extension"].toString();

    for (const auto& r : rules) {
        QJsonObject rule = r.toObject();
        HighlightingRule highlightingRule;
        highlightingRule.format = jsonToFormat(formats[rule["format"].toString()].toObject());

        if (rule.contains("words")) {
            const auto ruleWords = words[rule["words"].toString()].toArray();

            for (const auto& word : ruleWords) {
                highlightingRule.pattern = QRegularExpression(rule["pattern"].toString().arg(word.toString()));
                highlightingRules.append(highlightingRule);
            }

        } else {
            highlightingRule.pattern = QRegularExpression(rule["pattern"].toString());
            highlightingRules.append(highlightingRule);
        }

    }

    for (const auto& b : blocks) {
        QJsonObject block = b.toObject();
        if (block["name"].toString() == "SingleLineComment") {
            commentStartExpression = QRegularExpression(block["start"].toString());
            commentEndExpression = QRegularExpression(block["end"].toString());
            multiLineCommentFormat = jsonToFormat(formats[block["format"].toString()].toObject());
        }
    }

    valid = true;
}

QTextCharFormat Highlighter::jsonToFormat(const QJsonObject& obj) {
    QTextCharFormat format;

    if (obj.contains("foreground")) {
        QColor color(obj["foreground"].toString());
        format.setForeground(QBrush(color));
    }

    if (obj.contains("bold")) {
        format.setFontWeight(QFont::Bold);
    }

    return format;
}
