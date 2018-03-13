#include "Editor.h"
#include <QtCore>

Editor::Editor(QString filePath, QWidget* parent) :
        QTextEdit(parent),
        filePath(filePath) {
    readFile();
}

void Editor::readFile() {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file" << filePath;
        return;
    }

    QTextStream in(&file);
    append(in.readAll());
}
