#include "Global.h"
#include "Const.h"
#include "Settings.h"
#include <QtGui>

static QString s_systemRustupHome;
static QString s_systemCargoHome;
static QFont s_faFont;

void Global::init() {
    s_systemRustupHome = qEnvironmentVariable(Const::Environment::RustupHome);
    s_systemCargoHome = qEnvironmentVariable(Const::Environment::CargoHome);

    int id = QFontDatabase::addApplicationFont(":/resources/font/fontawesome/Font-Awesome-5-Free-Solid-900.otf");
    if (id < 0) {
        qWarning() << "Failed to load FontAwesome!";
    }

    s_faFont.setFamily(QFontDatabase::applicationFontFamilies(id).at(0));
    s_faFont.setPixelSize(16);
}

QString Global::workspacePath() {
    QString workspace = Settings::value("workspace").toString();
    return workspace.isEmpty() ? QDir::homePath() + "/" + Const::Window::WorkspaceDir : workspace;
}

QString Global::systemRustupHome() {
    return s_systemRustupHome;
}

QString Global::systemCargoHome() {
    return s_systemCargoHome;
}

QFont Global::fontAwesomeFont() {
    return s_faFont;
}
