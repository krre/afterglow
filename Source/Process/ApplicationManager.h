#pragma once
#include "ProcessManager.h"

class ApplicationManager : public ProcessManager {
    Q_OBJECT
public:
    explicit ApplicationManager(QObject* parent = nullptr);
    ~ApplicationManager();

signals:
    void consoleMessage(const QString& message, bool start = false);

private slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus) override;
};
