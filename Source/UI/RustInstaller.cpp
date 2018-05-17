#include "RustInstaller.h"
#include "ui_RustInstaller.h"
#include "Core/Settings.h"
#include <QtWidgets>

RustInstaller::RustInstaller(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::RustInstaller) {
    ui->setupUi(this);
    ui->lineEditRustup->setText(Settings::getValue("rustup.path").toString());
}

RustInstaller::~RustInstaller() {
    delete ui;
}

void RustInstaller::on_pushButtonBrowseRustup_clicked() {
    QString path = QFileDialog::getOpenFileName(this);
    if (!path.isEmpty()) {

        ui->lineEditRustup->setText(path);
        Settings::setValue("rustup.path", path);
    }
}
