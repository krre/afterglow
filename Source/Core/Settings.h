#pragma once
#include <QJsonValue>

class Settings {

public:
    static void init();
    static void flush();

    static void setValue(const QString& path, const QJsonValue& value);
    static QJsonValue getValue(const QString& path);

    static QString getPrefsPath();
    static void updateRustEnvironmentVariables();

private:
    static void cleanupDeprecated(QJsonObject& src, QJsonObject& dst);
    static void appendNew(QJsonObject& src, QJsonObject& dst);
    static void modifyJsonValue(QJsonObject& obj, const QString& path, const QJsonValue& newValue);

    static QJsonObject storage;
    static QString prefsPath;
};
