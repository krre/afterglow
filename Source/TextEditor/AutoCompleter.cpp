#include "AutoCompleter.h"
#include "TextEditor.h"

AutoCompleter::AutoCompleter(const QStringList& list, QObject* parent) : QCompleter(list, parent) {
    setCompletionMode(QCompleter::PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);

    QObject::connect(this, SIGNAL(activated(QString)), this, SLOT(onActivate(QString)));
}

void AutoCompleter::setTextEditor(TextEditor* editor) {
    this->editor = editor;
    setWidget(editor);
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
