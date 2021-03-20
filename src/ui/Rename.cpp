#include "Rename.h"
#include "ui_Rename.h"
#include <QtWidgets>

Rename::Rename(const QString& path, QWidget* parent) :
        QDialog(parent),
        ui(new Ui::Rename) {
    ui->setupUi(this);
    ui->lineEdit->setFocus();
    ui->buttonBox->buttons().at(0)->setEnabled(false);
    setFixedHeight(height());

    QFileInfo fi(path);
    name = fi.fileName();
    ui->lineEdit->setText(name);
    ui->label->setText(QString("Rename %1 %2?").arg(fi.isDir() ? "directory" : "file", name));
}

Rename::~Rename() {
    delete ui;
}

QString Rename::getName() const {
    return name;
}

void Rename::on_buttonBox_accepted() {
    name = ui->lineEdit->text();
}

void Rename::on_buttonBox_rejected() {
    name = QString();
}

void Rename::on_lineEdit_textChanged(const QString& text) {
    ui->buttonBox->buttons().at(0)->setEnabled(!text.isEmpty() && text != name);
}
