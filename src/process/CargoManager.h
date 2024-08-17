#pragma once
#include "ProcessManager.h"
#include <QTime>

class ProjectProperties;

class CargoManager : public ProcessManager {
    Q_OBJECT
public:
    enum class Target {
        Binary,
        Library
    };

    enum class BuildTarget {
        Debug,
        Release
    };

    explicit CargoManager(ProjectProperties* projectProperties, QObject* parent = nullptr);
    ~CargoManager();

    void createProject(Target target, const QString& path);
    void build();
    void run();
    void check();
    void clean();
    void doc();

    void setProjectPath(const QString& path);
    QString projectPath() const;

signals:
    void consoleMessage(const QString& message, bool html = false, bool start = false);
    void projectCreated(const QString& path);

private slots:
    void onReadyReadStandardOutput(const QString& data) override;
    void onReadyReadStandardError(const QString& data) override;
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus) override;

private:
    void prepareAndStart(const QStringList& arguments);
    void addBuildRunArguments(QStringList& arguments);

    enum class CommandStatus {
        None,
        New,
        Build,
        Run
    };

    void coloredOutputMessage(const QString& message, bool start = false);

    ProjectProperties* m_projectProperties = nullptr;
    CommandStatus m_commandStatus = CommandStatus::None;
    QString m_projectPath;
};
