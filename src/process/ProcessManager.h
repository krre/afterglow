#pragma once
#include <QProcess>

class ProcessManager : public QObject {
public:
    explicit ProcessManager(QObject* parent = nullptr);
    QProcess* process() const;
    void stop();

protected slots:
    virtual void onReadyReadStandardOutput(const QString& data);
    virtual void onReadyReadStandardError(const QString& data);
    virtual void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    virtual void onErrorOccurred(QProcess::ProcessError error);

protected:
    QString errorToString(QProcess::ProcessError error);

private:
    QProcess* m_process = nullptr;
};
