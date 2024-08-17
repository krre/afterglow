#pragma once
#include <QPlainTextEdit>
#include <QPoint>

class SyntaxHighlightManager;
class Highlighter;
class AutoCompleter;

class TextEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit TextEditor(QString filePath, SyntaxHighlightManager* syntaxHighlightManager, QWidget* parent = nullptr);

    QString filePath() const { return m_filePath; }
    void setFilePath(const QString& filePath);

    void setAutoCompleter(AutoCompleter* completer);
    AutoCompleter* autoCompleter() const { return m_completer; }

    void saveFile();
    QString modifiedName() const;

    QPoint cursorPosition();
    void setCursorPosition(const QPoint& pos);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    QString textUnderCursor() const;
    int leftMargin() const;

    void joinLines();
    void duplicateLine();
    void cutLine();

    void increaseIndent();
    void decreaseIndent();

    void toggleSingleLineComment();
    void toggleBlockComment();

    void openAutoCompleter();
    void goToLine(int line);
    void cleanTrailingWhitespace();

signals:
    void documentModified(TextEditor* editor);
    void focusChanged(bool focus);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    void readFile();
    void autoindent();
    void extendSelectionToBeginOfComment();

    SyntaxHighlightManager* m_syntaxHighlightManager = nullptr;
    QWidget* m_lineNumberArea = nullptr;
    Highlighter* m_highlighter = nullptr;
    QString m_filePath;
    AutoCompleter* m_completer = nullptr;
};
