#include "Global.h"
#include "Constants.h"
#include "Settings.h"
#include <QtCore>

static QString systemRustupHome;
static QString systemCargoHome;

void Global::init() {
    systemRustupHome = qEnvironmentVariable(Constants::Environment::RUSTUP_HOME);
    systemCargoHome = qEnvironmentVariable(Constants::Environment::CARGO_HOME);
}

QString Global::getWorkspacePath() {
    QString workspace = Settings::getValue("workspace").toString();
    return workspace.isEmpty() ? QDir::homePath() + "/" + Constants::Window::WORKSPACE_DIRECTORY : workspace;
}

QString Global::getSystemRustupHome() {
    return systemRustupHome;
}

QString Global::getSystemCargoHome() {
    return systemCargoHome;
}
