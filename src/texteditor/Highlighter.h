#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter {
public:
    explicit Highlighter(const QString& fileExt, QTextDocument* parent);
    const QString& getLangName() const { return m_langName; }
    const QString& getLangExt() const { return m_langExt; }
    bool isValid() const { return m_valid; }

    static bool hasExtension(const QString& ext);

protected:
    void highlightBlock(const QString& text) override;

private:
    void loadRules(const QString& fileExt);
    QTextCharFormat jsonToFormat(const QJsonObject& obj);

    enum class BlockState {
        MultilineCommentBegin,
        MultilineCommentEnd
    };

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    bool m_valid = false;

    QString m_langName;
    QString m_langExt;

    QVector<HighlightingRule> m_highlightingRules;

    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;

    QTextCharFormat m_multiLineCommentFormat;
};
