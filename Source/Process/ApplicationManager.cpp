#include "ApplicationManager.h"
#include <QtCore>

ApplicationManager::ApplicationManager() {

}

ApplicationManager::~ApplicationManager() {

}

void ApplicationManager::start(const QString& command) {
    emit consoleMessage(QString("Starting %1...\n").arg(command), true);
    getProcess()->start(command);
}

void ApplicationManager::onReadyReadStandardOutput(const QString& data) {
    emit consoleMessage(data);
}

void ApplicationManager::onReadyReadStandardError(const QString& data) {
    emit consoleMessage(data);
}

void ApplicationManager::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString message = QString("The process %1 %2 with code %3\n")
            .arg(getProcess()->program())
            .arg(exitStatus == QProcess::NormalExit ? "finished normally" : "crashed")
            .arg(exitCode);
    emit consoleMessage(message);
}

void ApplicationManager::onErrorOccurred(QProcess::ProcessError error) {
    emit consoleMessage(QString("Process error: %1").arg(errorToString(error)));
}
