#include "ProcessManager.h"
#include <QDebug>

ProcessManager::ProcessManager(QObject* parent) : QObject(parent) {
    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, [=] {
        const QByteArray& data = process->readAllStandardOutput();
        const QString& output = outputCodec->toUnicode(data.constData(), data.length(), &outputCodecState);
        onReadyReadStandardOutput(output);
    });

    connect(process, &QProcess::readyReadStandardError, [=] {
        const QByteArray& data = process->readAllStandardError();
        const QString& output = outputCodec->toUnicode(data.constData(), data.length(), &errorCodecState);
        onReadyReadStandardError(output);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=] (int exitCode, QProcess::ExitStatus exitStatus) { onFinished(exitCode, exitStatus); });

    connect(process, &QProcess::errorOccurred, this, &ProcessManager::onErrorOccurred);
}

QProcess* ProcessManager::getProcess() {
    return process;
}

void ProcessManager::stop() {
    process->close();
}

void ProcessManager::onReadyReadStandardOutput(const QString& data) {
    Q_UNUSED(data)
}

void ProcessManager::onReadyReadStandardError(const QString& data) {
    Q_UNUSED(data)
}

void ProcessManager::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
}

void ProcessManager::onErrorOccurred(QProcess::ProcessError error) {
    Q_UNUSED(error)
}

QString ProcessManager::errorToString(QProcess::ProcessError error) {
    switch (error) {
        case QProcess::FailedToStart:
            return tr("Failed to start");
        case QProcess::Crashed:
            return tr("Crashed");
        case QProcess::Timedout:
            return tr("Timedout");
        case QProcess::WriteError:
            return tr("Write error");
        case QProcess::ReadError:
            return tr("Read error");
        case QProcess::UnknownError:
            return tr("Unknown error");
        default:
            break;
    }

    return QString();
}
