#include "ProjectProperties.h"
#include "ui_ProjectProperties.h"
#include <QtCore>

ProjectProperties::ProjectProperties(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::ProjectProperties) {
    ui->setupUi(this);
}

ProjectProperties::~ProjectProperties() {
    delete ui;
}

CargoManager::BuildTarget ProjectProperties::getBuildTarget() const {
    return static_cast<CargoManager::BuildTarget>(ui->comboBoxTarget->currentIndex());
}

void ProjectProperties::setBuildTarget(CargoManager::BuildTarget buildTarget) {
    ui->comboBoxTarget->setCurrentIndex(static_cast<int>(buildTarget));
}

void ProjectProperties::setProject(const QString& projectPath) {
    this->projectPath = projectPath;
    updateMetadata();
}

const QString ProjectProperties::getRunTarget() const {
    return metadata["target_directory"].toString() + "/"
            + (getBuildTarget() == CargoManager::BuildTarget::Debug ? "debug" : "release") + "/"
            + ui->comboBoxRun->currentText();
}

void ProjectProperties::reset() {
    ui->comboBoxTarget->setCurrentIndex(0);
    projectPath = QString();
    ui->comboBoxRun->clear();
}

void ProjectProperties::updateMetadata() {
    QString manifestPath = projectPath + "/Cargo.toml";
    QProcess process;
    QStringList arguments;
    arguments << "metadata";
    arguments << "--format-version" << "1";
    arguments << "--manifest-path" << manifestPath;
    arguments << "--no-deps";
    process.start("cargo", arguments);
    process.waitForFinished();

    QJsonDocument doc(QJsonDocument::fromJson(process.readAllStandardOutput()));
    metadata = doc.object();

    QJsonArray targets = metadata["packages"].toArray().at(0).toObject()["targets"].toArray();
    for (int i = 0; i < targets.size(); i++) {
        ui->comboBoxRun->addItem(targets.at(i).toObject()["name"].toString());
    }
}
