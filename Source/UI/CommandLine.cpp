#include "CommandLine.h"
#include <QtWidgets>

CommandLine::CommandLine(QWidget* parent) : QLineEdit(parent) {

}

void CommandLine::run() {
    if (history.isEmpty() || history.last() != text()) {
        history.append(text());
        counter = history.count();
    }
    setText("");
}

void CommandLine::focusInEvent(QFocusEvent* event) {
    Q_UNUSED(event)
    emit focusReceived();
    QLineEdit::focusInEvent(event);
}

void CommandLine::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Up && !history.isEmpty()) {
        counter = qMax(0, counter - 1);
        setText(history.at(counter));
    } else if (event->key() == Qt::Key_Down && !history.isEmpty()) {
        counter = qMin(counter + 1, history.count());
        if (counter < history.count()) {
            setText(history.at(counter));
        } else {
            setText("");
        }
    } else {
        QLineEdit::keyPressEvent(event);
    }
}
