#pragma once
#include <QDialog>

namespace Ui {
    class InstallToolchain;
}

class InstallToolchain : public QDialog {
    Q_OBJECT

public:
    explicit InstallToolchain(QWidget* parent = 0);
    ~InstallToolchain();

private:
    Ui::InstallToolchain* ui;
};
