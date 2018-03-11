#pragma once
#include <QDialog>

namespace Ui {
    class NewCargoProject;
}

class NewCargoProject : public QDialog {
    Q_OBJECT

public:
    explicit NewCargoProject(QWidget* parent = 0);
    ~NewCargoProject();

private slots:
    void on_pushButtonDirectory_clicked();
    void on_buttonBox_accepted();
    void adjustAcceptedButton();

private:
    Ui::NewCargoProject* ui;
};
