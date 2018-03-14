#include "CargoManager.h"
#include <QtCore>

CargoManager::CargoManager(QObject* parent) : QObject(parent) {
    process = new QProcess(this);
    process->setProgram("cargo");
    connect(process, &QProcess::readyReadStandardOutput, this, &CargoManager::onReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError, this, &CargoManager::onReadyReadStandardError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
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
    prepareAndStart(arguments);
    commandStatus = CommandStatus::New;
    setProjectPath(path);
}

void CargoManager::build() {
    QStringList arguments;
    arguments << "build";
    prepareAndStart(arguments);
    commandStatus = CommandStatus::Build;
}

void CargoManager::run() {
    QStringList arguments;
    arguments << "run";
    prepareAndStart(arguments);
    commandStatus = CommandStatus::Run;
}

void CargoManager::setProjectPath(const QString& path) {
    projectPath = path;
    process->setWorkingDirectory(path);
}

void CargoManager::onReadyReadStandardOutput() {
    QByteArray data = process->readAllStandardOutput();
    QString output = outputCodec->toUnicode(data.constData(), data.length(), &outputCodecState);
    emit outputMessage(output);
}

void CargoManager::onReadyReadStandardError() {
    QByteArray data = process->readAllStandardError();
    QString output = outputCodec->toUnicode(data.constData(), data.length(), &errorCodecState);
    emit outputMessage(output);
}

void CargoManager::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)

    if (exitCode) {
        return;
    }

    switch (commandStatus) {
    case CommandStatus::New:
        emit projectCreated(process->arguments().last());
        break;
    default:
        break;
    }
    QString finishedMessage = "The process " + process->program() +
        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed");
    timedOutputMessage(finishedMessage);
    timedOutputMessage(QString("Elapsed time: %1 ms").arg(measureTime.elapsed()));
    emit outputMessage();
}

void CargoManager::prepareAndStart(const QStringList& arguments) {
    process->setArguments(arguments);

    QString message = "Starting: cargo";
    for (const auto& argument : arguments) {
        message += " " + argument;
    }

    timedOutputMessage(message);
    measureTime.start();
    process->start();
}

void CargoManager::timedOutputMessage(const QString& message) {
    QString timedMessage = QTime::currentTime().toString("hh:mm:ss: ") + message;
    emit outputMessage(timedMessage);
}
