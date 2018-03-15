#include "LineNumberArea.h"
#include "Editor.h"

LineNumberArea::LineNumberArea(Editor* editor) : QWidget(editor) {
    this->editor = editor;
}

QSize LineNumberArea::sizeHint() const {
    return QSize(editor->getLineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    editor->lineNumberAreaPaintEvent(event);
}
