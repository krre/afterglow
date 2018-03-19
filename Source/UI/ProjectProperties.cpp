#include "ProjectProperties.h"
#include "ui_ProjectProperties.h"

ProjectProperties::ProjectProperties(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::ProjectProperties) {
    ui->setupUi(this);
}

ProjectProperties::~ProjectProperties() {
    delete ui;
}

CargoManager::BuildTarget ProjectProperties::getTarget() const {
    return static_cast<CargoManager::BuildTarget>(ui->comboBoxTarget->currentIndex());
}

void ProjectProperties::setTarget(CargoManager::BuildTarget target) {
    ui->comboBoxTarget->setCurrentIndex(static_cast<int>(target));
}
