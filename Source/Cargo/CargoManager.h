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

    explicit CargoManager(QObject* parent = nullptr);
    ~CargoManager();

    void createProject(ProjectTemplate projectTemplate, const QString& path);
    void build();
    void run();

    void setProjectPath(const QString& path);

signals:
    void outputMessage(const QString& message = QString());
    void projectCreated(const QString& path);

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void prepareAndStart(const QStringList& arguments);

    enum class CommandStatus {
        New,
        Build,
        Run
    };

    void timedOutputMessage(const QString& message);

    QProcess* process;
    QTime measureTime;
    CommandStatus commandStatus;
    QTextCodec* outputCodec = QTextCodec::codecForLocale();
    QTextCodec::ConverterState outputCodecState;
    QTextCodec::ConverterState errorCodecState;
    QString projectPath;
};
