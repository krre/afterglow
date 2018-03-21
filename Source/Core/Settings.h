#pragma once
#include <QJsonValue>
#include <QVector>

namespace Settings {
    const char version[] = "version";
    const char window[] = "window";
    const char width[] = "width";
    const char height[] = "height";

    void init();
    void flush();
    void setValue(const QString& path, const QJsonValue& value);
}
