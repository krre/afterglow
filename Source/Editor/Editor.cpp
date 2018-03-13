#include "Editor.h"
#include <QtCore>

Editor::Editor(QString filePath, QWidget* parent) :
        QTextEdit(parent),
        filePath(filePath) {
    readFile();
}

void Editor::saveFile() {
    QFile file(filePath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toPlainText();
    } else {
        qWarning() << "Failed to open file for writing" << filePath;
    }
}

void Editor::readFile() {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
    } else {
        qWarning() << "Failed to open file for reading" << filePath;
    }
}
