#pragma once
#include <QTextEdit>

class Editor : public QTextEdit {
    Q_OBJECT
public:
    explicit Editor(QString filePath, QWidget* parent = nullptr);
    QString getFilePath() const { return filePath; }
    void saveFile();
    QString getModifiedName() const;

signals:
    void documentModified(Editor* editor);

private:
    void readFile();

    QString filePath;
};
