#include "InstallToolchain.h"
#include "ui_InstallToolchain.h"
#include <QtCore>

InstallToolchain::InstallToolchain(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::InstallToolchain) {
    ui->setupUi(this);
    setFixedHeight(height());

    loadHosts();
}

InstallToolchain::~InstallToolchain() {
    delete ui;
}

void InstallToolchain::on_comboBoxChannel_currentIndexChanged(int index) {
    ui->lineEditChannel->setEnabled(index == ui->comboBoxChannel->count() - 1);
}

void InstallToolchain::on_comboBoxHost_currentIndexChanged(int index) {
    ui->lineEditHost->setEnabled(index == ui->comboBoxHost->count() - 1);

}

void InstallToolchain::loadHosts() {
#if defined(Q_OS_LINUX)
    ui->comboBoxHost->addItem("x86_64-unknown-linux-gnu");
    ui->comboBoxHost->addItem("i686-unknown-linux-gnu");
#elif defined(Q_OS_WIN)
    ui->comboBoxHost->addItem("x86_64-pc-windows-gnu");
    ui->comboBoxHost->addItem("x86_64-pc-windows-msvc");
    ui->comboBoxHost->addItem("i686-pc-windows-gnu");
    ui->comboBoxHost->addItem("i686-pc-windows-msvc");
#endif
    ui->comboBoxHost->addItem(tr("Custom"));
    on_comboBoxHost_currentIndexChanged(0);
}
