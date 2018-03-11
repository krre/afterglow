#include "NewCargoProject.h"
#include "ui_NewCargoProject.h"

NewCargoProject::NewCargoProject(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewCargoProject) {
    ui->setupUi(this);
    setFixedHeight(height());
}

NewCargoProject::~NewCargoProject() {
    delete ui;
}
