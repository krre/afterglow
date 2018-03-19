#include "Global.h"
#include "Constants.h"
#include <QtCore>

QString Global::getPortableSettingsPath() {
    return QCoreApplication::applicationDirPath() + "/" + Constants::APP_SETTINGS_NAME;
}

QString Global::getDefaultWorkspacePath() {
    return QDir::homePath() + "/" + Constants::WORKSPACE_DIRECTORY;
}
