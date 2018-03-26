#include "ApplicationManager.h"
#include <QtCore>

ApplicationManager::ApplicationManager(QObject* parent) : ProcessManager(parent) {

}

ApplicationManager::~ApplicationManager() {

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

    QString finishedMessage = "The process " + getProcess()->program() +
        (exitStatus == QProcess::NormalExit ? " finished normally" : " crashed");
    emit consoleMessage(finishedMessage);
}
