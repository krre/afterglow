#pragma once
#include <QPlainTextEdit>

class Highlighter;
class QCompleter;

class Editor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit Editor(QString filePath, QWidget* parent = nullptr);

    QString getFilePath() const { return filePath; }
    void setFilePath(const QString& filePath);

    void setCompleter(QCompleter* completer);
    QCompleter* getCompleter() const { return completer; }

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
    void documentModified(Editor* editor);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void insertCompletion(const QString& completion);

private:
    void readFile();
    QString textUnderCursor() const;

    QWidget* lineNumberArea;
    Highlighter* highlighter;
    QString filePath;
    QCompleter* completer;
};
