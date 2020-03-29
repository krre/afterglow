#include "Settings.h"
#include "Const.h"
#include "Global.h"
#include <QtCore>

static QJsonObject storage = QJsonObject();
static QString prefsPath = QString();
static bool reseted = false;

void Settings::init() {
    prefsPath = QCoreApplication::applicationDirPath() + "/" + Const::App::PrefsName;

    QFile resPrefsFile(":/resources/prefs.json");
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

    QFile workPrefsFile(prefsPath);
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

    updateRustEnvironmentVariables();
}

void Settings::flush() {
    if (reseted) {
        QFile::remove(prefsPath);
        return;
    }

    QFile file(QCoreApplication::applicationDirPath() + "/" + Const::App::PrefsName);
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

QString Settings::getPrefsPath() {
    return prefsPath;
}

void Settings::updateRustEnvironmentVariables() {
    QString homePath = QDir::homePath();

    QString rustupHome = getValue("environment.rustupHome").toString();

    if (!rustupHome.isEmpty()) {
        qputenv(Const::Environment::RustupHome, rustupHome.toUtf8());
    } else if (Global::getSystemRustupHome().isEmpty()) {
        qputenv(Const::Environment::RustupHome, QString(homePath + "/.rustup").toUtf8());
    }

    QString cargoHome = getValue("environment.cargoHome").toString();
    if (!cargoHome.isEmpty()) {
        qputenv(Const::Environment::CargoHome, cargoHome.toUtf8());
    } else if (Global::getSystemCargoHome().isEmpty()) {
        qputenv(Const::Environment::CargoHome, QString(homePath + "/.cargo").toUtf8());
    }

    QString path = qEnvironmentVariable(Const::Environment::Path);
    QString separator;
#if defined(Q_OS_WIN)
    separator = ";";
#else
    separator = ":";
#endif
    path += separator + qEnvironmentVariable(Const::Environment::CargoHome) + "/bin";
    qputenv(Const::Environment::Path, QDir::toNativeSeparators(path).toUtf8());
}

void Settings::reset() {
    reseted = true;
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
