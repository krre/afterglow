#include "Options.h"
#include "ui_Options.h"
#include "Core/Global.h"
#include "Core/Settings.h"
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
    ui->lineEditWorkspace->setText(Global::getWorkspacePath());
    ui->checkBoxSession->setChecked(Settings::getValue("gui.session.restore").toBool());
}

void Options::writeSettings() {
    Settings::setValue("workspace", ui->lineEditWorkspace->text());
    Settings::setValue("gui.session.restore", ui->checkBoxSession->isChecked());
}
