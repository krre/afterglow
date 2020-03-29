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
    if (ui->lineEditChannel->isEnabled()) {
        ui->lineEditChannel->setFocus();
    }
}

void InstallToolchain::on_comboBoxHost_currentIndexChanged(int index) {
    ui->lineEditHost->setEnabled(index == ui->comboBoxHost->count() - 1);
    if (ui->lineEditHost->isEnabled()) {
        ui->lineEditHost->setFocus();
    }
}

void InstallToolchain::on_buttonBox_accepted() {
    QString channel = ui->comboBoxChannel->currentText();
    if (ui->comboBoxChannel->currentIndex() == ui->comboBoxChannel->count() - 1) {
        channel = ui->lineEditChannel->text();
    }

    QString date = ui->lineEditDate->text();

    QString host = ui->comboBoxHost->currentText();
    if (ui->comboBoxHost->currentIndex() == ui->comboBoxHost->count() - 1) {
        host = ui->lineEditHost->text();
    }

    toolchain = channel + "-" + (!date.isEmpty() ? QString("%1-").arg(date) : "") + host;
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
