#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit Highlighter(const QString& fileExt, QTextDocument* parent);
    const QString& getLang() const { return lang; }
    const QString& getLangExt() const { return langExt; }
    bool isValid() const { return valid; }

    static bool hasExtension(const QString& ext);

protected:
    void highlightBlock(const QString& text) override;

private:
    void loadRules(const QString& fileExt);

    enum class BlockState {
        MultilineCommentBegin,
        MultilineCommentEnd
    };

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    bool valid = false;

    QString lang;
    QString langExt;

    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};
