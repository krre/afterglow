#include "ProjectProperties.h"
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>

ProjectProperties::ProjectProperties(QWidget* parent) : QWidget(parent) {
    setWindowTitle(tr("ProjectProperties"));
    resize(400, 300);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Target:")));

    m_targetComboBox = new QComboBox;
    m_targetComboBox->addItem(tr("Debug"));
    m_targetComboBox->addItem(tr("Release"));

    verticalLayout->addWidget(m_targetComboBox, 0, Qt::AlignLeft);
    verticalLayout->addWidget(new QLabel(tr("Run:")));

    m_runComboBox = new QComboBox;
    verticalLayout->addWidget(m_runComboBox, 0, Qt::AlignLeft);

    verticalLayout->addWidget(new QLabel(tr("Arguments:")));

    m_argumentsLineEdit = new QLineEdit;
    verticalLayout->addWidget(m_argumentsLineEdit);

    verticalLayout->addStretch();

    setLayout(verticalLayout);
}

CargoManager::BuildTarget ProjectProperties::buildTarget() const {
    return static_cast<CargoManager::BuildTarget>(m_targetComboBox->currentIndex());
}

void ProjectProperties::setBuildTarget(CargoManager::BuildTarget buildTarget) {
    m_targetComboBox->setCurrentIndex(static_cast<int>(buildTarget));
}

void ProjectProperties::setProject(const QString& projectPath) {
    m_projectPath = projectPath;
    updateMetadata();
}

QString ProjectProperties::arguments() const {
    return m_argumentsLineEdit->text();
}

QStringList ProjectProperties::argumentsList() const {
    return m_argumentsLineEdit->text().split(' ');
}

void ProjectProperties::setArguments(const QString& arguments) {
    m_argumentsLineEdit->setText(arguments);
}

const QString ProjectProperties::runTarget() const {
    return m_metadata["target_directory"].toString() + "/"
            + (buildTarget() == CargoManager::BuildTarget::Debug ? "debug" : "release") + "/"
           + m_runComboBox->currentText();
}

void ProjectProperties::reset() {
    m_targetComboBox->setCurrentIndex(0);
    m_projectPath = QString();
    m_runComboBox->clear();
}

void ProjectProperties::updateMetadata() {
    QString manifestPath = m_projectPath + "/Cargo.toml";
    QProcess process;
    process.start("cargo", { "metadata", "--format-version", "1", "--manifest-path", manifestPath, "--no-deps" });
    process.waitForFinished();

    m_metadata = QJsonDocument::fromJson(process.readAllStandardOutput()).object();
    QJsonArray targets = m_metadata["packages"].toArray().at(0).toObject()["targets"].toArray();

    for (int i = 0; i < targets.size(); ++i) {
        if (targets.at(i).toObject()["kind"].toArray().at(0).toString() == "bin") {
            m_runComboBox->addItem(targets.at(i).toObject()["name"].toString());
        }
    }
}
