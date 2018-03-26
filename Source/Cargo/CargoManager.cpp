#include "CargoManager.h"
#include <QtCore>

CargoManager::CargoManager(QObject* parent) : QObject(parent) {
    cargoProcess = new QProcess(this);
    cargoProcess->setProgram("cargo");
    connect(cargoProcess, &QProcess::readyReadStandardOutput, this, &CargoManager::onReadyReadStandardOutput);
    connect(cargoProcess, &QProcess::readyReadStandardError, this, &CargoManager::onReadyReadStandardError);
    connect(cargoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) { onFinished(exitCode, exitStatus); });
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
    prepareAndStart(arguments);
}

void CargoManager::run(BuildTarget buildTarget, const QString& runTarget) {
    commandStatus = CommandStatus::Run;
    build(buildTarget);
    this->runTarget = runTarget;
}

void CargoManager::clean() {
    QStringList arguments;
    arguments << "clean";
    prepareAndStart(arguments);
}

void CargoManager::setProjectPath(const QString& path) {
    projectPath = path;
    cargoProcess->setWorkingDirectory(path);
}

void CargoManager::onReadyReadStandardOutput() {
    QByteArray data = cargoProcess->readAllStandardOutput();
    QString output = outputCodec->toUnicode(data.constData(), data.length(), &outputCodecState);
    emit cargoMessage(output);
}

void CargoManager::onReadyReadStandardError() {
    QByteArray data = cargoProcess->readAllStandardError();
    QString output = outputCodec->toUnicode(data.constData(), data.length(), &errorCodecState);
    emit cargoMessage(output);
}

void CargoManager::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)

    if (exitCode) {
        return;
    }

    switch (commandStatus) {
    case CommandStatus::New:
        emit projectCreated(cargoProcess->arguments().last());
        break;
    case CommandStatus::Run:
        qDebug() << runTarget;
        break;
    default:
        break;
    }
    QString finishedMessage = "The process " + cargoProcess->program() +
        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed");
    timedOutputMessage(finishedMessage);
    timedOutputMessage(QString("Elapsed time: %1 ms").arg(measureTime.elapsed()));
}

void CargoManager::prepareAndStart(const QStringList& arguments) {
    cargoProcess->setArguments(arguments);

    QString message = "Starting: cargo";
    for (const auto& argument : arguments) {
        message += " " + argument;
    }

    timedOutputMessage(message, true);
    measureTime.start();
    cargoProcess->start();
}

void CargoManager::timedOutputMessage(const QString& message, bool start) {
    QString timedMessage = QTime::currentTime().toString("hh:mm:ss: ") + message + "\n";
    emit cargoMessage(timedMessage, start);
}
