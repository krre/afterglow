#include "ProcessManager.h"
#include <QStringConverter>
#include <QDebug>

ProcessManager::ProcessManager(QObject* parent) : QObject(parent) {
    m_process = new QProcess(this);

    connect(m_process, &QProcess::readyReadStandardOutput, this, [=, this] {
        QByteArray data = m_process->readAllStandardOutput();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        onReadyReadStandardOutput(output);
    });

    connect(m_process, &QProcess::readyReadStandardError, this, [=, this] {
        QByteArray data = m_process->readAllStandardError();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        onReadyReadStandardError(output);
    });

    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
        [=, this] (int exitCode, QProcess::ExitStatus exitStatus) { onFinished(exitCode, exitStatus); });

    connect(m_process, &QProcess::errorOccurred, this, &ProcessManager::onErrorOccurred);
}

QProcess* ProcessManager::process() const {
    return m_process;
}

void ProcessManager::stop() {
    m_process->close();
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
