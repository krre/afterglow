#include "NewProject.h"
#include "ui_NewProject.h"
#include "core/Global.h"
#include "process/CargoManager.h"
#include <QtWidgets>

NewProject::NewProject(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewProject) {
    ui->setupUi(this);
    setFixedHeight(height());

    ui->lineEditDirectory->setText(Global::workspacePath());

    connect(ui->lineEditName, &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);
    connect(ui->lineEditDirectory, &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);

    adjustAcceptedButton();
}

NewProject::~NewProject() {
    delete ui;
}

void NewProject::on_pushButtonDirectory_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEditDirectory->setText(dirPath);
    }
}

void NewProject::on_buttonBox_accepted() {
    projectTemplate = static_cast<CargoManager::ProjectTemplate>(ui->comboBoxTemplate->currentIndex());
    projectPath = ui->lineEditDirectory->text() + "/" + ui->lineEditName->text();
}

void NewProject::adjustAcceptedButton() {
    ui->buttonBox->buttons().at(0)->setEnabled(!ui->lineEditName->text().isEmpty() && !ui->lineEditDirectory->text().isEmpty());
}
