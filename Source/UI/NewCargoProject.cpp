#include "NewCargoProject.h"
#include "ui_NewCargoProject.h"
#include "Core/Global.h"
#include "Cargo/CargoProcessor.h"
#include <QtCore>
#include <QtWidgets>

NewCargoProject::NewCargoProject(CargoProcessor* cargoProcessor, QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewCargoProject),
        cargoProcessor(cargoProcessor) {
    ui->setupUi(this);
    setFixedHeight(height());

    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    ui->lineEditDirectory->setText(settings.value("Path/Workspace", Global::getDefaultWorkspacePath()).toString());

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
    CargoProcessor::Template projectTemplate = static_cast<CargoProcessor::Template>(ui->comboBoxTemplate->currentIndex());
    QString projectPath = ui->lineEditDirectory->text() + "/" + ui->lineEditName->text();
    cargoProcessor->createProject(projectTemplate, projectPath);
}

void NewCargoProject::adjustAcceptedButton() {
    ui->buttonBox->buttons().at(0)->setEnabled(!ui->lineEditName->text().isEmpty() && !ui->lineEditDirectory->text().isEmpty());
}
