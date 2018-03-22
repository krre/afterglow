#include "SelectWorkspace.h"
#include "ui_SelectWorkspace.h"
#include "Core/Constants.h"
#include "Core/Global.h"
#include "Core/Settings.h"
#include <QtWidgets>

SelectWorkspace::SelectWorkspace(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::SelectWorkspace) {
    ui->setupUi(this);
    setFixedHeight(height());
    ui->lineEdit->setText(Global::getWorkspacePath());
}

SelectWorkspace::~SelectWorkspace() {
    delete ui;
}

void SelectWorkspace::on_pushButtonBrowse_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEdit->setText(dirPath);
    }
}

void SelectWorkspace::on_buttonBox_accepted() {
    Settings::setValue("workspace", ui->lineEdit->text());
}

void SelectWorkspace::on_lineEdit_textChanged(const QString& text) {
    ui->buttonBox->buttons().at(0)->setEnabled(!text.isEmpty());
}
