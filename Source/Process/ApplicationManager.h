#pragma once
#include "ProcessManager.h"
#include "Core/Singleton.h"

class ProjectProperties;

class ApplicationManager : public ProcessManager, public Singleton<ApplicationManager> {
    Q_OBJECT
public:
    explicit ApplicationManager(ProjectProperties* projectProperties);
    ~ApplicationManager();
    void start();

signals:
    void consoleMessage(const QString& message, bool start = false);

private slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus) override;
    void onErrorOccurred(QProcess::ProcessError error);

private:
    ProjectProperties* projectProperties;
};
