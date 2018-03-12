#include "CargoProcessor.h"
#include <QtCore>

CargoProcessor::CargoProcessor(QObject* parent) : QObject(parent) {
    process = new QProcess(this);
    process->setProgram("cargo");
    connect(process, &QProcess::readyReadStandardOutput, this, &CargoProcessor::onReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError, this, &CargoProcessor::onReadyReadStandardError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) { onFinished(exitCode, exitStatus); });
}

CargoProcessor::~CargoProcessor() {

}

void CargoProcessor::createProject(ProjectTemplate projectTemplate, const QString& path) {
    QStringList arguments;
    arguments << "new";
    if (projectTemplate == ProjectTemplate::Binary) {
        arguments << "--bin";
    } else if (projectTemplate == ProjectTemplate::Library) {
        arguments << "--lib";
    }

    arguments << path;
    process->setArguments(arguments);
    process->start();
    commandStatus = CommandStatus::New;
}

void CargoProcessor::onReadyReadStandardOutput() {
    QByteArray data = process->readAllStandardOutput();
    QString output = outputCodec->toUnicode(data.constData(), data.length(), &outputCodecState);

    qDebug() << "readAllStandardOutput";
    qDebug() << output;
}

void CargoProcessor::onReadyReadStandardError() {
    QByteArray data = process->readAllStandardError();
    QString output = outputCodec->toUnicode(data.constData(), data.length(), &errorCodecState);

    qDebug() << "readAllStandardError";
    qDebug() << output;
}

void CargoProcessor::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    qDebug() << exitCode << exitStatus;

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
//    QString finishedMessage = "The process " + process->program() +
//        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed");
//    timedOutputMessage(finishedMessage);
//    timedOutputMessage(QString("Elapsed time: %1 ms").arg(measureTime.elapsed()));
}

void CargoProcessor::timedOutputMessage(const QString& message) {
    QString timedMessage = QTime::currentTime().toString("hh:mm:ss: ") + message;
    outputMessage(timedMessage);
}
