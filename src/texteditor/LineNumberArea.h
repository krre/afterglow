#pragma once
#include <QWidget>

class TextEditor;

class LineNumberArea : public QWidget {
public:
    LineNumberArea(TextEditor* editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TextEditor* m_editor = nullptr;
};
