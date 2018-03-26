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
    if (exitCode) {
        return;
    }

    QString message = "The process " + getProcess()->program() +
        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed") + "\n";
    emit consoleMessage(message);
}
