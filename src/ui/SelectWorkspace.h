#pragma once
#include <QDialog>

namespace Ui {
    class SelectWorkspace;
}

class SelectWorkspace : public QDialog {
    Q_OBJECT

public:
    explicit SelectWorkspace(QWidget* parent = 0);
    ~SelectWorkspace();

private slots:
    void on_pushButtonBrowse_clicked();
    void on_buttonBox_accepted();
    void on_lineEdit_textChanged(const QString& text);

private:
    Ui::SelectWorkspace* ui;
};
