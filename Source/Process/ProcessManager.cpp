#include "ProcessManager.h"

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
        [=](int exitCode, QProcess::ExitStatus exitStatus) { onFinished(exitCode, exitStatus); });
}

QProcess* ProcessManager::getProcess() {
    return process;
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
