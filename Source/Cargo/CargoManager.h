#pragma once
#include <QObject>
#include <QProcess>
#include <QTime>
#include <QTextCodec>

class CargoManager : public QObject {
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
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void prepareAndStart(const QStringList& arguments);

    enum class CommandStatus {
        None,
        New,
        Build,
        Run
    };

    void timedOutputMessage(const QString& message, bool start = false);

    QProcess* cargoProcess;
    QTime measureTime;
    CommandStatus commandStatus = CommandStatus::None;
    QTextCodec* outputCodec = QTextCodec::codecForLocale();
    QTextCodec::ConverterState outputCodecState;
    QTextCodec::ConverterState errorCodecState;
    QString projectPath;
    QString runTarget;
};
