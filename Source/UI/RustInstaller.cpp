#include "RustInstaller.h"
#include "ui_RustInstaller.h"

RustInstaller::RustInstaller(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::RustInstaller) {
    ui->setupUi(this);
}

RustInstaller::~RustInstaller() {
    delete ui;
}
