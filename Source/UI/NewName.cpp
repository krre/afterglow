#include "NewName.h"
#include "ui_NewName.h"
#include <QtWidgets>

NewName::NewName(const QString& title, QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewName) {
    setWindowTitle(title);
    ui->setupUi(this);
    ui->lineEditName->setFocus();
    ui->buttonBox->buttons().at(0)->setEnabled(false);
    setFixedHeight(height());
}

NewName::~NewName() {
    delete ui;
}

QString NewName::getName() const {
    return name;
}

void NewName::on_buttonBox_accepted() {
    name = ui->lineEditName->text();
}

void NewName::on_lineEditName_textChanged(const QString& text) {
    Q_UNUSED(text)
    ui->buttonBox->buttons().at(0)->setEnabled(!text.isEmpty());
}
