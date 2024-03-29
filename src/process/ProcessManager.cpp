#include "ProcessManager.h"
#include <QStringConverter>

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

    connect(m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &ProcessManager::onFinished);
    connect(m_process, &QProcess::errorOccurred, this, &ProcessManager::onErrorOccurred);
}

QProcess* ProcessManager::process() const {
    return m_process;
}

void ProcessManager::stop() {
    m_process->close();
}

void ProcessManager::onReadyReadStandardOutput(const QString& data [[maybe_unused]]) {

}

void ProcessManager::onReadyReadStandardError(const QString& data [[maybe_unused]]) {

}

void ProcessManager::onFinished(int exitCode [[maybe_unused]], QProcess::ExitStatus exitStatus [[maybe_unused]]) {

}

void ProcessManager::onErrorOccurred(QProcess::ProcessError error [[maybe_unused]]) {

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
