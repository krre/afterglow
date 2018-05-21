#include "InstallToolchain.h"
#include "ui_InstallToolchain.h"

InstallToolchain::InstallToolchain(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::InstallToolchain) {
    ui->setupUi(this);
}

InstallToolchain::~InstallToolchain() {
    delete ui;
}
