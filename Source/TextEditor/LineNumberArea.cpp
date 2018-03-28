#include "LineNumberArea.h"
#include "TextEditor.h"

LineNumberArea::LineNumberArea(TextEditor* editor) : QWidget(editor) {
    this->editor = editor;
}

QSize LineNumberArea::sizeHint() const {
    return QSize(editor->getLineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    editor->lineNumberAreaPaintEvent(event);
}
