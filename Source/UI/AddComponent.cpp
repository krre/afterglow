#include "AddComponent.h"
#include "ui_AddComponent.h"

AddComponent::AddComponent(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AddComponent) {
    ui->setupUi(this);
}

AddComponent::~AddComponent() {
    delete ui;
}
