#include "Options.h"
#include "ui_Options.h"
#include "Core/Global.h"
#include <QtWidgets>

Options::Options(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::Options) {
    ui->setupUi(this);
    adjustSize();
    resize(600, height());
    readSettings();
}

Options::~Options() {
    delete ui;
}

void Options::on_pushButtonCargo_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEditCargo->setText(dirPath);
    }
}

void Options::on_pushButtonWorkspace_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEditWorkspace->setText(dirPath);
    }
}

void Options::on_buttonBox_accepted() {
    writeSettings();
}

void Options::readSettings() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    ui->lineEditCargo->setText(settings.value("Path/Cargo").toString());
    ui->lineEditWorkspace->setText(settings.value("Path/workspace", Global::getDefaultWorkspacePath()).toString());
    ui->checkBoxSession->setChecked(settings.value("MainWindow/restoreSession", true).toBool());
}

void Options::writeSettings() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    settings.setValue("Path/cargo", ui->lineEditCargo->text());
    settings.setValue("Path/workspace", ui->lineEditWorkspace->text());
    settings.setValue("MainWindow/restoreSession", ui->checkBoxSession->isChecked());
}
