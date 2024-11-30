#pragma once
#include <QVariant>

class Settings {
public:
    static void init();
    static void flush();

    static void setValue(const QString& path, const QVariant& value);
    static QVariant value(const QString& path);

    static QString prefsPath();
    static void updateRustEnvironmentVariables();
    static void reset();

private:
    static void cleanupDeprecated(QVariantMap& src, QVariantMap& dst);
    static void appendNew(QVariantMap& src, QVariantMap& dst);
    static void modifyValue(QVariantMap& obj, const QString& path, const QVariant& newValue);
};
