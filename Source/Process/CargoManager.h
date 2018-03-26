#pragma once
#include "ProcessManager.h"
#include <QTime>

class CargoManager : public ProcessManager {
    Q_OBJECT
public:
    enum class ProjectTemplate {
        Binary,
        Library
    };

    enum class BuildTarget {
        Debug,
        Release
    };

    explicit CargoManager(QObject* parent = nullptr);
    ~CargoManager();

    void createProject(ProjectTemplate projectTemplate, const QString& path);
    void build(BuildTarget buildTarget);
    void run(BuildTarget buildTarget, const QString& runTarget);
    void clean();

    void setProjectPath(const QString& path);

signals:
    void cargoMessage(const QString& message, bool start = false);
    void applicationMessage(const QString& message = QString());
    void projectCreated(const QString& path);

private slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus) override;

private:
    void prepareAndStart(const QStringList& arguments);

    enum class CommandStatus {
        None,
        New,
        Build,
        Run
    };

    void timedOutputMessage(const QString& message, bool start = false);

    QTime measureTime;
    CommandStatus commandStatus = CommandStatus::None;
    QString projectPath;
    QString runTarget;
};
