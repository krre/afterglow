#include "CargoProcessor.h"
#include <QtCore>

CargoProcessor::CargoProcessor(QObject* parent) : QObject(parent) {
    process = new QProcess(this);
    process->setProgram("cargo");
    connect(process, &QProcess::readyRead, this, &CargoProcessor::onReadyRead);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) { onFinished(exitCode, exitStatus); });
}

CargoProcessor::~CargoProcessor() {

}

void CargoProcessor::createProject(Template projectTemplate, const QString& path) {
    QStringList arguments;
    arguments << "new";
    if (projectTemplate == Template::Binary) {
        arguments << "--bin";
    } else if (projectTemplate == Template::Library) {
        arguments << "--lib";
    }

    arguments << path;
    process->setArguments(arguments);
    process->start();
}

void CargoProcessor::onReadyRead() {
    QTextStream in(process->readAll());
    while (!in.atEnd()) {
        emit outputMessage(in.readLine());
        qDebug() << in.readLine();
    }
}

void CargoProcessor::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    qDebug() << exitCode << exitStatus;
//    QString finishedMessage = "The process " + process->program() +
//        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed");
//    timedOutputMessage(finishedMessage);
//    timedOutputMessage(QString("Elapsed time: %1 ms").arg(measureTime.elapsed()));
}

void CargoProcessor::timedOutputMessage(const QString& message) {
    QString timedMessage = QTime::currentTime().toString("hh:mm:ss: ") + message;
    outputMessage(timedMessage);
}
