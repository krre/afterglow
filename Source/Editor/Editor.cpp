#include "Editor.h"
#include <QtCore>

Editor::Editor(QString filePath, QWidget* parent) :
        QTextEdit(parent),
        filePath(filePath) {
    connect(this, &Editor::textChanged, [=](){
        emit documentModified(this);
    });

    readFile();
}

void Editor::saveFile() {
    QFile file(filePath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toPlainText();
        document()->setModified(false);
        documentModified(this);
    } else {
        qWarning() << "Failed to open file for writing" << filePath;
    }
}

QString Editor::getModifiedName() const {
    QFileInfo fi(filePath);
    return fi.fileName() + (document()->isModified() ? "*" : "");
}

void Editor::readFile() {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
    } else {
        qWarning() << "Failed to open file for reading" << filePath;
    }
}
