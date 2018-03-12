#include "Global.h"
#include "Constants.h"
#include <QtCore>

QString Global::getPortableSettingsPath() {
    return QCoreApplication::applicationDirPath() + "/" + APP_SETTINGS_NAME;
}

QString Global::getDefaultWorkspacePath() {
    return QDir::homePath() + "/" + WORKSPACE_DIRECTORY;
}
