#pragma once
#include <QWidget>

class TextEditor;

class LineNumberArea : public QWidget {
    Q_OBJECT
public:
    LineNumberArea(TextEditor* editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TextEditor* editor = nullptr;
};
