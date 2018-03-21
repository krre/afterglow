#pragma once
#include <QObject>
#include <QJsonDocument>

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject* parent = nullptr);
    static void init();
    static void flush();

private:
    static QJsonDocument workDoc;
};
