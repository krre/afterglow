#pragma once
#include <QDialog>

namespace Ui {
    class GoToLine;
}

class GoToLine : public QDialog {
    Q_OBJECT

public:
    explicit GoToLine(QWidget* parent = nullptr);
    ~GoToLine();
    int line() const { return m_line; }
    bool isValid() const { return valid; }

private slots:
    void on_lineEdit_textChanged(const QString& text);

private:
    Ui::GoToLine* ui;
    int m_line = 0;
    bool valid = false;
};
