#include "CargoManager.h"
#include "ui/ProjectProperties.h"
#include "core/Settings.h"
#include "core/Const.h"
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
    addBuildRunArguments(arguments);
    commandStatus = CommandStatus::Build;
    prepareAndStart(arguments);
}

void CargoManager::run() {
    QStringList arguments;
    arguments << "run";
    addBuildRunArguments(arguments);
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
    process()->setWorkingDirectory(path);
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
            emit projectCreated(process()->arguments().last());
            break;
        default:
            break;
    }

    QString message = QString("%1 %2 with code %3")
            .arg(process()->program(), exitStatus == QProcess::NormalExit ? "finished" : "crashed")
            .arg(exitCode);
    coloredOutputMessage(message);
}

void CargoManager::prepareAndStart(const QStringList& arguments) {
    QString cargoPath = Settings::value("cargo.path").toString();
    process()->setProgram(cargoPath.isEmpty() ? "cargo" : cargoPath);
    process()->setArguments(arguments);

    QString message = process()->program();
    for (const auto& argument : arguments) {
        message += " " + argument;
    }

    message += "<br>";

    coloredOutputMessage(message, true);
    process()->start();
}

void CargoManager::addBuildRunArguments(QStringList& arguments) {
    if (Settings::value("debug.compilerMessageFormat").toString() == "json") {
        arguments << "--message-format";
        arguments << "json";
    }

    if (projectProperties->getBuildTarget() == BuildTarget::Release) {
        arguments << "--release";
    }
}

void CargoManager::coloredOutputMessage(const QString& message, bool start) {
    QString coloredMessage = QString("<font color=%1>%2</font>")
            .arg(Const::Color::Message, message);
    emit consoleMessage(coloredMessage, true, start);
}
