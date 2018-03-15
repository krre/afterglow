#include "Editor.h"
#include <QtGui>

Editor::Editor(QString filePath, QWidget* parent) :
        QPlainTextEdit(parent),
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

void Editor::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Tab) {
        insertPlainText(QString(4, ' '));
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QPlainTextEdit::keyPressEvent(event);
        QTextCursor cursor = textCursor();
        int row = cursor.blockNumber();
//        int column = cursor.positionInBlock();
        if (row > 0) {
            QTextBlock block = document()->findBlockByLineNumber(row - 1);
            int count = 0;
            while (block.text().at(count) == ' ') {
                count++;
            }

            if (count > 0) {
                insertPlainText(QString(count, ' '));
            }
        }
    } else {
        QPlainTextEdit::keyPressEvent(event);
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
