#include "CargoManager.h"
#include "ApplicationManager.h"
#include <QtCore>

CargoManager::CargoManager(QObject* parent) : ProcessManager(parent) {
    getProcess()->setProgram("cargo");
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

void CargoManager::build(BuildTarget buildTarget) {
    QStringList arguments;
    arguments << "build";
    if (buildTarget == BuildTarget::Release) {
        arguments << "--release";
    }
    commandStatus = CommandStatus::Build;
    prepareAndStart(arguments);
}

void CargoManager::run(BuildTarget buildTarget, const QString& runTarget) {
    build(buildTarget);
    commandStatus = CommandStatus::Run;
    this->runTarget = runTarget;
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
    if (exitCode) {
        return;
    }

    switch (commandStatus) {
    case CommandStatus::New:
        emit projectCreated(getProcess()->arguments().last());
        break;
    case CommandStatus::Run:
        ApplicationManager::getInstance()->start(runTarget);
        break;
    default:
        break;
    }
    QString finishedMessage = "The process " + getProcess()->program() +
        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed");
    timedOutputMessage(finishedMessage);
    timedOutputMessage(QString("Elapsed time: %1 ms").arg(measureTime.elapsed()));
}

void CargoManager::prepareAndStart(const QStringList& arguments) {
    getProcess()->setArguments(arguments);

    QString message = "Starting: cargo";
    for (const auto& argument : arguments) {
        message += " " + argument;
    }

    timedOutputMessage(message, true);
    measureTime.start();
    getProcess()->start();
}

void CargoManager::timedOutputMessage(const QString& message, bool start) {
    QString timedMessage = QTime::currentTime().toString("hh:mm:ss: ") + message + "\n";
    emit consoleMessage(timedMessage, start);
}
