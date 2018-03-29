#include "AutoCompleter.h"
#include "TextEditor.h"
#include <QtWidgets>

AutoCompleter::AutoCompleter(const QStringList& list, QObject* parent) : QCompleter(list, parent) {
    setCompletionMode(QCompleter::PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);

    QObject::connect(this, SIGNAL(activated(QString)), this, SLOT(onActivate(QString)));
}

void AutoCompleter::setTextEditor(TextEditor* editor) {
    this->editor = editor;
    setWidget(editor);
}

void AutoCompleter::open(QKeyEvent* event) {
    Q_UNUSED(event)
//    const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
//    if (ctrlOrShift && event->text().isEmpty()) {
//        return;
//    }

//    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
//    bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString prefix = editor->textUnderCursor();

//    if ((hasModifier || event->text().isEmpty() || prefix.length() < 3
//                      || eow.contains(event->text().right(1)))) {
//        popup()->hide();
//        return;
//    }

    if (prefix != completionPrefix()) {
        setCompletionPrefix(prefix);
        popup()->setCurrentIndex(completionModel()->index(0, 0));
    }
    QRect cr = editor->cursorRect();
    cr.setX(cr.x() + editor->leftMargin());
    cr.setWidth(popup()->sizeHintForColumn(0) + popup()->verticalScrollBar()->sizeHint().width());

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
    arguments << QString::number(cursor.blockNumber() + 1);
    arguments << QString::number(cursor.columnNumber() + 1);
    arguments << tmpPath;

    qDebug() << arguments;

    process.start("racer", arguments);
    process.waitForFinished();

    file.remove();

    QString error = process.readAllStandardError();
    if (!error.isEmpty()) {
        qWarning() << "Error running Racer:" << error;
        return;
    }

    QString result = process.readAllStandardOutput();
    if (result.isEmpty()) return;

    qDebug() << result;

    //    complete(cr);
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
