#include "SetOverride.h"
#include "ui_SetOverride.h"
#include "Core/Utils.h"
#include <QtWidgets>

SetOverride::SetOverride(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::SetOverride) {
    ui->setupUi(this);
    ui->lineEdit->setFocus();
    ui->buttonBox->buttons().at(0)->setEnabled(false);

    QStringList list = Utils::getListFromConsole("rustup toolchain list");
    for (const QString& toolchain : list) {
        ui->comboBox->addItem(toolchain);
    }
}

SetOverride::~SetOverride() {
    delete ui;
}

void SetOverride::on_pushButtonBrowse_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEdit->setText(dirPath);
    }
}

void SetOverride::on_buttonBox_accepted() {
    directory = ui->lineEdit->text();
    toolchain = ui->comboBox->currentText();
}

void SetOverride::on_lineEdit_textChanged(const QString& text) {
    ui->buttonBox->buttons().at(0)->setEnabled(!text.isEmpty());
}
