#include "CommandLine.h"
#include <QtWidgets>

CommandLine::CommandLine(QWidget* parent) : QLineEdit(parent) {

}

void CommandLine::run() {
    if (m_history.isEmpty() || m_history.last() != text()) {
        m_history.append(text());
        m_counter = m_history.count();
    }
    setText("");
}

void CommandLine::focusInEvent(QFocusEvent* event) {
    Q_UNUSED(event)
    emit focusReceived();
    QLineEdit::focusInEvent(event);
}

void CommandLine::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Up && !m_history.isEmpty()) {
        m_counter = qMax(0, m_counter - 1);
        setText(m_history.at(m_counter));
    } else if (event->key() == Qt::Key_Down && !m_history.isEmpty()) {
        m_counter = qMin(m_counter + 1, m_history.count());
        if (m_counter < m_history.count()) {
            setText(m_history.at(m_counter));
        } else {
            setText("");
        }
    } else {
        QLineEdit::keyPressEvent(event);
    }
}
