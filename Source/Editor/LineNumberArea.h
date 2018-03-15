#pragma once
#include <QWidget>

class Editor;

class LineNumberArea : public QWidget {
    Q_OBJECT

public:
    LineNumberArea(Editor* editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Editor* editor;
};
