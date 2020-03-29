#include "Global.h"
#include "Const.h"
#include "Settings.h"
#include <QtGui>

static QString systemRustupHome;
static QString systemCargoHome;
static QFont faFont;

void Global::init() {
    systemRustupHome = qEnvironmentVariable(Const::Environment::RustupHome);
    systemCargoHome = qEnvironmentVariable(Const::Environment::CargoHome);

    int id = QFontDatabase::addApplicationFont(":/Resources/Font/FontAwesome/Font-Awesome-5-Free-Solid-900.otf");
    if (id < 0) {
        qWarning() << "Failed to load FontAwesome!";
    }

    faFont.setFamily(QFontDatabase::applicationFontFamilies(id).at(0));
    faFont.setPixelSize(16);
}

QString Global::getWorkspacePath() {
    QString workspace = Settings::getValue("workspace").toString();
    return workspace.isEmpty() ? QDir::homePath() + "/" + Const::Window::WorkspaceDir : workspace;
}

QString Global::getSystemRustupHome() {
    return systemRustupHome;
}

QString Global::getSystemCargoHome() {
    return systemCargoHome;
}

QFont Global::getFontAwesomeFont() {
    return faFont;
}
