#pragma once
#include <QDialog>

namespace Ui {
    class RustInstaller;
}

class RustInstaller : public QDialog {
    Q_OBJECT

public:
    explicit RustInstaller(QWidget* parent = 0);
    ~RustInstaller();

private:
    Ui::RustInstaller* ui;
};
