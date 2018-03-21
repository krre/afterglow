#pragma once
#include <QJsonValue>
#include <QVector>

namespace Settings {
    const char version[] = "version";
    const char window[] = "window";
    const char width[] = "width";
    const char height[] = "height";

    class Writer {
    public:
        ~Writer();
        Writer* go(const QString& key);
        Writer* set(QJsonValue value);
        void finish();
    private:
        QVector<QString> path;
        QJsonValue value;
    };

    void init();
    void flush();
    void setValue(const QString& path, const QJsonValue& value);
}
