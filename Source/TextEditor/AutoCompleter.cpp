#include "AutoCompleter.h"
#include "TextEditor.h"
#include "Process/RlsManager.h"
#include <QtWidgets>

AutoCompleter::AutoCompleter(QObject* parent) : QCompleter(parent) {
    setCompletionMode(QCompleter::PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);

    listModel = new QStringListModel(this);
    setModel(listModel);

    QObject::connect(this, SIGNAL(activated(QString)), this, SLOT(onActivate(QString)));
}

void AutoCompleter::setTextEditor(TextEditor* editor) {
    this->editor = editor;
    setWidget(editor);
}

void AutoCompleter::open() {
    QString prefix = editor->textUnderCursor();

    if (prefix != completionPrefix()) {
        setCompletionPrefix(prefix);
        popup()->setCurrentIndex(completionModel()->index(0, 0));
    }

    QString tmpPath = QDir::tempPath() + "/racer.tmp";
    QFile file(tmpPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open temporary Racer file" << tmpPath;
        return;
    }

    file.write(editor->document()->toPlainText().toUtf8());
    file.close();

    QProcess process;
    QStringList arguments;
    arguments << "complete";

    QTextCursor cursor = editor->textCursor();
    int row = cursor.blockNumber();
    int column = cursor.columnNumber();
    arguments << QString::number(row + 1);
    arguments << QString::number(column + 1);
    arguments << tmpPath;

    RlsManager::completion(editor->getFilePath(), row, column);

    process.start("racer", arguments);
    process.waitForFinished();

    file.remove();

    QString error = process.readAllStandardError();
    if (!error.isEmpty()) {
        qWarning() << "Error running Racer:" << error;
        return;
    }

    QString result = process.readAllStandardOutput();
    QStringList rows = result.split('\n');
    QStringList words;

    for (int i = 0; i < rows.count(); i++) {
        if (rows.at(i).left(5) == "MATCH") {
            QString word = rows.at(i).split(',').at(0).split(' ').at(1);
            words << word;
        }
    }

    listModel->setStringList(words);
    listModel->sort(0);

    QRect cr = editor->cursorRect();
    cr.setX(cr.x() + editor->leftMargin());
    cr.setWidth(popup()->sizeHintForColumn(0) + popup()->verticalScrollBar()->sizeHint().width());

    complete(cr);
}

void AutoCompleter::onActivate(const QString& completion) {
    if (widget() != editor) {
        return;
    }

    QTextCursor cursor = editor->textCursor();
    int extra = completion.length() - completionPrefix().length();
    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);
    cursor.insertText(completion.right(extra));
    editor->setTextCursor(cursor);
}
