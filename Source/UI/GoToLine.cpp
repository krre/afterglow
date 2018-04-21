#include "GoToLine.h"
#include "ui_GoToLine.h"
#include <QtWidgets>

GoToLine::GoToLine(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::GoToLine) {
    ui->setupUi(this);
    setFixedHeight(height());
    ui->buttonBox->buttons().at(0)->setEnabled(false);
}

GoToLine::~GoToLine() {
    delete ui;
}

void GoToLine::on_lineEdit_textChanged(const QString& text) {
    bool ok;
    int value = text.toInt(&ok);
    line = ok ? value : -1;

    ui->buttonBox->buttons().at(0)->setEnabled(line != -1);
}
