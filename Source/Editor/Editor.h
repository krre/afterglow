#pragma once
#include <QPlainTextEdit>

class Highlighter;

class Editor : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit Editor(QString filePath, QWidget* parent = nullptr);
    QString getFilePath() const { return filePath; }
    void saveFile();
    QString getModifiedName() const;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int getLineNumberAreaWidth();

signals:
    void documentModified(Editor* editor);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    void readFile();

    QWidget* lineNumberArea;
    Highlighter* highlighter;
    QString filePath;
};
