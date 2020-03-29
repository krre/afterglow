#include "AutoCompleter.h"
#include "TextEditor.h"
#include "process/RlsManager.h"
#include <QtWidgets>

AutoCompleter::AutoCompleter(QObject* parent) : QCompleter(parent) {
    setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);

    listModel = new QStringListModel(this);
    setModel(listModel);

    tmpPath = QDir::tempPath() + "/racer.tmp";
    tmpFile.setFileName(tmpPath);

    connect(this, SIGNAL(activated(QString)), this, SLOT(onActivate(QString)));
    connect(RlsManager::getInstance(), &RlsManager::completionResult, this, &AutoCompleter::onCompletionResult);
}

void AutoCompleter::setTextEditor(TextEditor* editor) {
    this->editor = editor;
    setWidget(editor);
}

void AutoCompleter::open() {
    if (!tmpFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open temporary Racer file" << tmpPath;
        return;
    }

    tmpFile.write(editor->document()->toPlainText().toUtf8());
    tmpFile.close();

    QTextCursor cursor = editor->textCursor();
    int row = cursor.blockNumber();
    int column = cursor.columnNumber();

    RlsManager::completion(tmpPath, row, column);
//    RlsManager::completion(editor->getFilePath(), row, column);
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

void AutoCompleter::onCompletionResult(const QJsonArray& result) {
    tmpFile.remove();

    QStringList words;

    for (const QJsonValue& value : result) {
        words << value.toObject()["label"].toString();
    }

    listModel->setStringList(words);
    listModel->sort(0);

    QString prefix = editor->textUnderCursor();

    if (prefix != completionPrefix()) {
        setCompletionPrefix(prefix);
        popup()->setCurrentIndex(completionModel()->index(0, 0));
    }

    QRect cr = editor->cursorRect();
    cr.setX(cr.x() + editor->leftMargin());
    cr.setWidth(popup()->sizeHintForColumn(0) + popup()->verticalScrollBar()->sizeHint().width());

    complete(cr);
}
