#include "LineNumberArea.h"
#include "TextEditor.h"

LineNumberArea::LineNumberArea(TextEditor* editor) : QWidget(editor) {
    this->m_editor = editor;
}

QSize LineNumberArea::sizeHint() const {
    return QSize(m_editor->getLineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    m_editor->lineNumberAreaPaintEvent(event);
}
