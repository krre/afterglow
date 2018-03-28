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
    complete(cr); // popup it up!
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
