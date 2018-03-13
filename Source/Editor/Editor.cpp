#include "Editor.h"
#include <QtCore>

Editor::Editor(QString filePath, QWidget* parent) :
        QTextEdit(parent),
        filePath(filePath) {
    readFile();
}

void Editor::readFile() {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
    } else {
        qWarning() << "Failed to open file" << filePath;
    }
}
