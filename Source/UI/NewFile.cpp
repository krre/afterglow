#include "NewFile.h"
#include "ui_NewFile.h"
#include <QtWidgets>

NewFile::NewFile(const QString& projectPath, QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewFile) {
    ui->setupUi(this);
    ui->lineEditDirectory->setText(projectPath);
    ui->lineEditName->setFocus();
    setFixedHeight(height());
}

NewFile::~NewFile() {
    delete ui;
}

QString NewFile::getFilePath() const {
    return filePath;
}

void NewFile::on_pushButtonBrowse_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this, QString(), ui->lineEditDirectory->text());
    if (!dirPath.isEmpty()) {
        ui->lineEditDirectory->setText(dirPath);
    }
}

void NewFile::on_buttonBox_accepted() {
    filePath = ui->lineEditDirectory->text() + "/" + ui->lineEditName->text();
}

void NewFile::on_lineEditName_textChanged(const QString& text) {
    Q_UNUSED(text)
    changeOkButtonState();
}

void NewFile::on_lineEditDirectory_textChanged(const QString& text) {
    Q_UNUSED(text)
    changeOkButtonState();
}

void NewFile::changeOkButtonState() {
    ui->buttonBox->buttons().at(0)->setEnabled(!(ui->lineEditName->text().isEmpty()
                                                  || ui->lineEditDirectory->text().isEmpty()));
}
