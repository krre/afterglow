#include "Settings.h"
#include "Constants.h"
#include <QtCore>

QJsonObject Settings::storage = QJsonObject();

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
        if (!workPrefsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file" << workPrefsFile.fileName();
            return;
        }
        QJsonParseError err;
        storage = QJsonDocument::fromJson(workPrefsFile.readAll(), &err).object();
        if (err.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse JSON file" << workPrefsFile.fileName();
            qWarning() << "Error:" << err.errorString() << "offset:" << err.offset;
            return;
        }
        // Update preferences.
        QJsonObject src = resDoc.object();
        cleanupDeprecated(src, storage);
        appendNew(src, storage);
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

// Using:
// Settings::setValue("window.width", 42);
void Settings::setValue(const QString& path, const QJsonValue& value) {
    modifyJsonValue(storage, path, value);
}

// Using:
// int width = Settings::getValue("window.width").toInt();
QJsonValue Settings::getValue(const QString& path) {
    QStringList keys = path.split('.');
    QJsonObject obj = storage;
    for (int i = 0; i < keys.count() - 1; i++) {
        obj = obj[keys.at(i)].toObject();
    }

    return obj[keys.last()];
}

void Settings::cleanupDeprecated(QJsonObject& src, QJsonObject& dst) {
    for (QString key : dst.keys()) {
        if (src.contains(key)) {
            if (dst[key].isObject()) {
                QJsonObject srcObj = src[key].toObject();
                QJsonObject dstObj = dst[key].toObject();
                cleanupDeprecated(srcObj, dstObj);
                dst[key] = dstObj;
            }
        } else {
            dst.remove(key);
        }
    }
}

void Settings::appendNew(QJsonObject& src, QJsonObject& dst) {
    for (QString key : src.keys()) {
        if (dst.contains(key)) {
            if (src[key].isObject()) {
                QJsonObject srcObj = src[key].toObject();
                QJsonObject dstObj = dst[key].toObject();
                appendNew(srcObj, dstObj);
                dst[key] = dstObj;
            }
        } else {
            dst[key] = src[key];
        }
    }
}

void Settings::modifyJsonValue(QJsonObject& obj, const QString& path, const QJsonValue& newValue) {
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
