#include "Global.h"
#include "Constants.h"
#include "Settings.h"
#include <QtCore>

QString Global::getWorkspacePath() {
    QString workspace = Settings::getValue("workspace").toString();
    return workspace.isEmpty() ? QDir::homePath() + "/" + Constants::WORKSPACE_DIRECTORY : workspace;
}
