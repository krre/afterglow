#include "ApplicationManager.h"
#include "UI/ProjectProperties.h"
#include <QtCore>

ApplicationManager::ApplicationManager(ProjectProperties* projectProperties) :
    projectProperties(projectProperties) {

}

ApplicationManager::~ApplicationManager() {

}

void ApplicationManager::start() {
    const QString& command = projectProperties->getRunTarget();
    coloredOutputMessage(QString("Starting %1...\n").arg(command), true);
    getProcess()->start(command, projectProperties->getArgumentsList());
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
    coloredOutputMessage(message);
}

void ApplicationManager::onErrorOccurred(QProcess::ProcessError error) {
    emit consoleMessage(QString("Process error: %1\n").arg(errorToString(error)));
}

void ApplicationManager::coloredOutputMessage(const QString& message, bool start) {
    QString coloredMessage = QString("<font color=%1>%2</font>").arg("#0000FF").arg(message);
    emit consoleMessage(coloredMessage, start);
}
