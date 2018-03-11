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

private:
    Ui::NewCargoProject* ui;
};
