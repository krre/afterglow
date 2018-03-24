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
    void build(BuildTarget target);
    void run(BuildTarget target);
    void clean();

    void setProjectPath(const QString& path);

signals:
    void cargoMessage(const QString& message = QString());
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

    void timedOutputMessage(const QString& message);

    QProcess* process;
    QTime measureTime;
    CommandStatus commandStatus = CommandStatus::None;
    QTextCodec* outputCodec = QTextCodec::codecForLocale();
    QTextCodec::ConverterState outputCodecState;
    QTextCodec::ConverterState errorCodecState;
    QString projectPath;
};
