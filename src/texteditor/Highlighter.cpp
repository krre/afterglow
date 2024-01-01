#include "Highlighter.h"
#include "SyntaxHighlightManager.h"
#include <QtCore>

Highlighter::Highlighter(const QString& fileExt, QTextDocument* parent, SyntaxHighlightManager* syntaxHighlightManager) :
    QSyntaxHighlighter(parent), m_syntaxHighlightManager(syntaxHighlightManager) {
    loadRules(fileExt);
}

void Highlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : std::as_const(m_highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(static_cast<int>(BlockState::MultilineCommentBegin));

    int startIndex = 0;
    if (static_cast<BlockState>(previousBlockState()) != BlockState::MultilineCommentEnd) {
        startIndex = text.indexOf(m_commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match = m_commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(static_cast<int>(BlockState::MultilineCommentEnd));
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = text.indexOf(m_commentStartExpression, startIndex + commentLength);
    }
}

void Highlighter::loadRules(const QString& fileExt) {
    if (!m_syntaxHighlightManager->hasExtension(fileExt)) return;

    QJsonObject obj = m_syntaxHighlightManager->syntaxJson(fileExt);

    QJsonObject lang = obj["lang"].toObject();
    QJsonObject words = obj["words"].toObject();
    QJsonArray rules = obj["rules"].toArray();
    QJsonArray blocks = obj["blocks"].toArray();
    QJsonObject formats = obj["formats"].toObject();

    m_langName = lang["name"].toString();
    m_langExt = lang["extension"].toString();

    for (const auto& r : rules) {
        QJsonObject rule = r.toObject();
        HighlightingRule highlightingRule;
        highlightingRule.format = jsonToFormat(formats[rule["format"].toString()].toObject());

        if (rule.contains("words")) {
            const auto ruleWords = words[rule["words"].toString()].toArray();

            for (const auto& word : ruleWords) {
                highlightingRule.pattern = QRegularExpression(rule["pattern"].toString().arg(word.toString()));
                m_highlightingRules.append(highlightingRule);
            }

        } else {
            highlightingRule.pattern = QRegularExpression(rule["pattern"].toString());
            m_highlightingRules.append(highlightingRule);
        }

    }

    for (const auto& b : blocks) {
        QJsonObject block = b.toObject();
        if (block["name"].toString() == "SingleLineComment") {
            m_commentStartExpression = QRegularExpression(block["start"].toString());
            m_commentEndExpression = QRegularExpression(block["end"].toString());
            m_multiLineCommentFormat = jsonToFormat(formats[block["format"].toString()].toObject());
        }
    }

    m_valid = true;
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
