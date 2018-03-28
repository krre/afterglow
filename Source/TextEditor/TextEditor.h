#pragma once
#include <QPlainTextEdit>

class Highlighter;
class AutoCompleter;

class TextEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit TextEditor(QString filePath, QWidget* parent = nullptr);

    QString getFilePath() const { return filePath; }
    void setFilePath(const QString& filePath);

    void setAutoCompleter(AutoCompleter* completer);
    AutoCompleter* getAutoCompleter() const { return completer; }

    void saveFile();
    QString getModifiedName() const;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int getLineNumberAreaWidth();

    void commentUncommentLine();
    void joinLines();
    void autoindent();
    void insertTabSpaces();
    void removeTabSpaces();
    void autocomplete(QKeyEvent* event);

signals:
    void documentModified(TextEditor* editor);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    void readFile();
    QString textUnderCursor() const;

    QWidget* lineNumberArea;
    Highlighter* highlighter;
    QString filePath;
    AutoCompleter* completer;
};
