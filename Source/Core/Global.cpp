#include "Global.h"
#include "Constants.h"
#include <QtCore>

namespace Global {

namespace {
}

QString getPortableSettingsPath() {
    return QCoreApplication::applicationDirPath() + "/" + APP_SETTINGS_NAME;
}

QString getDefaultWorkspacePath() {
    return QDir::homePath() + "/" + WORKSPACE_DIRECTORY;
}

} // Global
