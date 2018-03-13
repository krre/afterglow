#include "NewCargoProject.h"
#include "ui_NewCargoProject.h"
#include "Core/Global.h"
#include "Cargo/CargoManager.h"
#include <QtWidgets>

NewCargoProject::NewCargoProject(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewCargoProject) {
    ui->setupUi(this);
    setFixedHeight(height());

    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    ui->lineEditDirectory->setText(settings.value("Path/workspace", Global::getDefaultWorkspacePath()).toString());

    connect(ui->lineEditName, &QLineEdit::textChanged, this, &NewCargoProject::adjustAcceptedButton);
    connect(ui->lineEditDirectory, &QLineEdit::textChanged, this, &NewCargoProject::adjustAcceptedButton);

    adjustAcceptedButton();
}

NewCargoProject::~NewCargoProject() {
    delete ui;
}

void NewCargoProject::on_pushButtonDirectory_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEditDirectory->setText(dirPath);
    }
}

void NewCargoProject::on_buttonBox_accepted() {
    projectTemplate = static_cast<CargoManager::ProjectTemplate>(ui->comboBoxTemplate->currentIndex());
    projectPath = ui->lineEditDirectory->text() + "/" + ui->lineEditName->text();
}

void NewCargoProject::adjustAcceptedButton() {
    ui->buttonBox->buttons().at(0)->setEnabled(!ui->lineEditName->text().isEmpty() && !ui->lineEditDirectory->text().isEmpty());
}
