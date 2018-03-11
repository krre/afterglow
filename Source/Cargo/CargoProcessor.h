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

private slots:
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void timedOutputMessage(const QString& message);

    QProcess* process;
    QTime measureTime;
};
