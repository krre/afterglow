#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit Highlighter(const QString& fileExt, QTextDocument* parent);
    const QString& getLangName() const { return langName; }
    const QString& getLangExt() const { return langExt; }
    bool isValid() const { return valid; }

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

    bool valid = false;

    QString langName;
    QString langExt;

    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat multiLineCommentFormat;
};
