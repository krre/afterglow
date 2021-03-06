#include "Options.h"
#include "ui_Options.h"
#include "core/Const.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

Options::Options(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::Options) {
    ui->setupUi(this);
    ui->pushButtonOpenPrefs->setText(tr("Open %1").arg(Const::App::PrefsName));

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

void Options::on_pushButtonOpenPrefs_clicked() {
    emit openPrefs();
}

void Options::on_pushButtonResetSettings_clicked() {
    int button = QMessageBox::question(this, tr("Reset Settings"), tr("Settings will be reseted to default.\n"
                                                                      "You are need restart application.\n"
                                                                      "Continue?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        Settings::reset();
    }
}

void Options::on_buttonBox_accepted() {
    writeSettings();
}

void Options::readSettings() {
    ui->lineEditWorkspace->setText(Global::workspacePath());
    ui->checkBoxSession->setChecked(Settings::value("gui.mainWindow.session.restore").toBool());
}

void Options::writeSettings() {
    Settings::setValue("workspace", ui->lineEditWorkspace->text());
    Settings::setValue("gui.mainWindow.session.restore", ui->checkBoxSession->isChecked());
}
