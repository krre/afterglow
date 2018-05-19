#include "CommandLine.h"
#include <QtWidgets>

CommandLine::CommandLine(QWidget* parent) : QLineEdit(parent) {

}

void CommandLine::focusInEvent(QFocusEvent* event) {
    Q_UNUSED(event)
    emit focusReceived();
    QLineEdit::focusInEvent(event);
}
