#include "Settings.h"
#include "Constants.h"
#include <QtCore>

namespace {
    QJsonObject storage = QJsonObject();

    void syncObjects(QJsonObject& src, QJsonObject& dst) {
        for (QString value : src.keys()) {
            if (dst.contains(value)) {
                if (src[value].isObject()) {
                    QJsonObject srcObj = src[value].toObject();
                    QJsonObject dstObj = dst[value].toObject();
                    syncObjects(srcObj, dstObj);
                    dst[value] = dstObj;
                }
            } else {
                dst[value] = src[value];
            }
        }
    }

    void modifyJsonValue(QJsonObject& obj, const QString& path, const QJsonValue& newValue) {
        const int indexOfDot = path.indexOf('.');
        const QString propertyName = path.left(indexOfDot);
        const QString subPath = indexOfDot > 0 ? path.mid(indexOfDot + 1) : QString();

        QJsonValue subValue = obj[propertyName];

        if (subPath.isEmpty()) {
            subValue = newValue;
        } else {
            QJsonObject obj = subValue.toObject();
            modifyJsonValue(obj, subPath, newValue);
            subValue = obj;
        }

        obj[propertyName] = subValue;
    }
}

void Settings::init() {
    QFile resPrefsFile(":/Resources/prefs.json");
    if (!resPrefsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file" << resPrefsFile.fileName();
        return;
    }

    QJsonParseError err;
    QJsonDocument resDoc(QJsonDocument::fromJson(resPrefsFile.readAll(), &err));
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON file" << resPrefsFile.fileName();
        qWarning() << "Error:" << err.errorString() << "offset:" << err.offset;
        return;
    }

    QFile workPrefsFile(QCoreApplication::applicationDirPath() + "/" + Constants::APP_PREFS_NAME);
    if (workPrefsFile.exists()) {
        // Update preferences.
        QJsonObject src = resDoc.object();
        syncObjects(src, storage);
    } else {
        // Create preferences from resources.
        storage = resDoc.object();
    }

    flush();
}

void Settings::flush() {
    QFile file(QCoreApplication::applicationDirPath() + "/" + Constants::APP_PREFS_NAME);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file" << file.fileName();
        return;
    }

    QJsonDocument doc(storage);
    file.write(doc.toJson());
}

Settings::Writer::~Writer() {
    finish();
}

Settings::Writer* Settings::Writer::go(const QString& key) {
    path.append(key);
    return this;
}

Settings::Writer* Settings::Writer::set(QJsonValue value) {
    this->value = value;
    return this;
}

void Settings::Writer::finish() {
    if (!path.count()) return;

    QJsonObject& obj = storage;
    for (int i = 0; i < path.count() - 1; i++) {
        obj = obj[path.at(i)].toObject();
    }

    obj[path.last()] = value;

    path.clear();
    value = QJsonValue();
}

// Using: Settings::setValue("window.width", 42);
void Settings::setValue(const QString& path, const QJsonValue& value) {
    modifyJsonValue(storage, path, value);
}
