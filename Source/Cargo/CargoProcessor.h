#pragma once
#include <QObject>
#include <QProcess>
#include <QTime>

class CargoProcessor : public QObject {
    Q_OBJECT
public:
    enum class Template {
        Binary,
        Library
    };

    explicit CargoProcessor(QObject* parent = nullptr);
    ~CargoProcessor();
    void createProject(Template projectTemplate, const QString& path);

signals:
    void outputMessage(const QString& message);
    void projectCreated(const QString& path);

private slots:
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:

    enum class CommandStatus {
        New,
        Build,
        Run
    };

    void timedOutputMessage(const QString& message);

    QProcess* process;
    QTime measureTime;
    CommandStatus commandStatus;
};
