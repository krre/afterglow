#pragma once
#include <QJsonValue>

class Settings {

public:
    static void init();
    static void flush();

    static void setValue(const QString& path, const QJsonValue& value);
    static QJsonValue getValue(const QString& path);

private:
    static void syncObjects(QJsonObject& src, QJsonObject& dst);
    static void modifyJsonValue(QJsonObject& obj, const QString& path, const QJsonValue& newValue);

    static QJsonObject storage;
};
