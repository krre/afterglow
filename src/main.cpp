#include "ui/MainWindow.h"
#include "ui/SelectWorkspace.h"
#include "core/Constants.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setOrganizationName(Const::App::Organization);
    app.setApplicationName(Const::App::Name);
    app.setApplicationVersion(Const::App::Version);

    Global::init();
    Settings::init();

    MainWindow window;
    window.show();

    if (Settings::value("workspace").toString().isEmpty()) {
        SelectWorkspace selectWorkspace(&window);
        selectWorkspace.exec();
    }

    return app.exec();
}
