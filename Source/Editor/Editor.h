#pragma once
#include <QTextEdit>

class Editor : public QTextEdit {
    Q_OBJECT
public:
    explicit Editor(QString filePath, QWidget* parent = nullptr);
    QString getFilePath() const { return filePath; }
    void saveFile();

private:
    void readFile();

    QString filePath;
};
