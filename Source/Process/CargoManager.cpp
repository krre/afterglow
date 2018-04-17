#include "CargoManager.h"
#include "UI/ProjectProperties.h"
#include "Core/Settings.h"
#include <QtCore>

CargoManager::CargoManager(ProjectProperties* projectProperties, QObject* parent) :
        ProcessManager(parent),
        projectProperties(projectProperties) {
}

CargoManager::~CargoManager() {

}

void CargoManager::createProject(ProjectTemplate projectTemplate, const QString& path) {
    QStringList arguments;
    arguments << "new";
    if (projectTemplate == ProjectTemplate::Binary) {
        arguments << "--bin";
    } else if (projectTemplate == ProjectTemplate::Library) {
        arguments << "--lib";
    }

    arguments << path;
    commandStatus = CommandStatus::New;
    prepareAndStart(arguments);
    setProjectPath(path);
}

void CargoManager::build() {
    QStringList arguments;
    arguments << "build";
    if (projectProperties->getBuildTarget() == BuildTarget::Release) {
        arguments << "--release";
    }
    commandStatus = CommandStatus::Build;
    prepareAndStart(arguments);
}

void CargoManager::run() {
    QStringList arguments;
    arguments << "run";
    if (projectProperties->getBuildTarget() == BuildTarget::Release) {
        arguments << "--release";
    }
    commandStatus = CommandStatus::Run;
    prepareAndStart(arguments);
}

void CargoManager::clean() {
    QStringList arguments;
    arguments << "clean";
    prepareAndStart(arguments);
}

void CargoManager::setProjectPath(const QString& path) {
    projectPath = path;
    getProcess()->setWorkingDirectory(path);
}

void CargoManager::onReadyReadStandardOutput(const QString& data) {
    emit consoleMessage(data);
}

void CargoManager::onReadyReadStandardError(const QString& data) {
    emit consoleMessage(data);
}

void CargoManager::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    switch (commandStatus) {
        case CommandStatus::New:
            emit projectCreated(getProcess()->arguments().last());
            break;
        default:
            break;
    }

    QString message = QString("%1 %2 with code %3")
            .arg(getProcess()->program())
            .arg(exitStatus == QProcess::NormalExit ? "finished" : "crashed")
            .arg(exitCode);
    coloredOutputMessage(message);
}

void CargoManager::prepareAndStart(const QStringList& arguments) {
    QString cargoPath = Settings::getValue("cargo.path").toString();
    getProcess()->setProgram(cargoPath.isEmpty() ? "cargo" : cargoPath);
    getProcess()->setArguments(arguments);

    QString message = getProcess()->program();
    for (const auto& argument : arguments) {
        message += " " + argument;
    }

    message += "<br>";

    coloredOutputMessage(message, true);
    getProcess()->start();
}

void CargoManager::coloredOutputMessage(const QString& message, bool start) {
    QString coloredMessage = QString("<font color=%1>%2</font>")
            .arg("#0000FF")
            .arg(message);
    emit consoleMessage(coloredMessage, true, start);
}
