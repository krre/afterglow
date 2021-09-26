#include "ProjectProperties.h"
#include <QtWidgets>

ProjectProperties::ProjectProperties(QWidget* parent) : QWidget(parent) {
    setWindowTitle(tr("ProjectProperties"));
    resize(400, 300);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Target:")));

    targetComboBox = new QComboBox;
    targetComboBox->addItem(tr("Debug"));
    targetComboBox->addItem(tr("Release"));

    verticalLayout->addWidget(targetComboBox, 0, Qt::AlignLeft);
    verticalLayout->addWidget(new QLabel(tr("Run:")));

    runComboBox = new QComboBox;
    verticalLayout->addWidget(runComboBox, 0, Qt::AlignLeft);

    verticalLayout->addWidget(new QLabel(tr("Arguments:")));

    argumentsLineEdit = new QLineEdit;
    verticalLayout->addWidget(argumentsLineEdit);

    verticalLayout->addStretch();

    setLayout(verticalLayout);
}

CargoManager::BuildTarget ProjectProperties::buildTarget() const {
    return static_cast<CargoManager::BuildTarget>(targetComboBox->currentIndex());
}

void ProjectProperties::setBuildTarget(CargoManager::BuildTarget buildTarget) {
    targetComboBox->setCurrentIndex(static_cast<int>(buildTarget));
}

void ProjectProperties::setProject(const QString& projectPath) {
    this->projectPath = projectPath;
    updateMetadata();
}

QString ProjectProperties::arguments() const {
    return argumentsLineEdit->text();
}

QStringList ProjectProperties::argumentsList() const {
    return argumentsLineEdit->text().split(' ');
}

void ProjectProperties::setArguments(const QString& arguments) {
    argumentsLineEdit->setText(arguments);
}

const QString ProjectProperties::runTarget() const {
    return metadata["target_directory"].toString() + "/"
            + (buildTarget() == CargoManager::BuildTarget::Debug ? "debug" : "release") + "/"
            + runComboBox->currentText();
}

void ProjectProperties::reset() {
    targetComboBox->setCurrentIndex(0);
    projectPath = QString();
    runComboBox->clear();
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
        if (targets.at(i).toObject()["kind"].toArray().at(0).toString() == "bin") {
            runComboBox->addItem(targets.at(i).toObject()["name"].toString());
        }
    }
}
