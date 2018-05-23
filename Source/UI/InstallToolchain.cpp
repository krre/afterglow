#include "InstallToolchain.h"
#include "ui_InstallToolchain.h"

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
}
