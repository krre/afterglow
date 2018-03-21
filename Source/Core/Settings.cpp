#include "Settings.h"
#include "Constants.h"
#include <QtCore>

QJsonDocument Settings::workDoc = QJsonDocument();

Settings::Settings(QObject* parent) : QObject(parent) {

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
        QJsonObject dst = workDoc.object();
        syncObjects(src, dst);
        workDoc.setObject(dst);
    } else {
        // Create preferences from resources.
        workDoc = resDoc;
    }

    flush();
}

void Settings::flush() {
    QFile workPrefsFile(QCoreApplication::applicationDirPath() + "/" + Constants::APP_PREFS_NAME);
    if (!workPrefsFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file" << workPrefsFile.fileName();
        return;
    }

    workPrefsFile.write(workDoc.toJson());
}

void Settings::syncObjects(QJsonObject& src, QJsonObject& dst) {
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
