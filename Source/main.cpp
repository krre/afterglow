#include "UI/MainWindow.h"
#include "UI/SelectWorkspace.h"
#include "Core/Const.h"
#include "Core/Global.h"
#include "Core/Settings.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName(Const::App::NAME);
    app.setApplicationVersion(Const::App::VERSION);

    Global::init();
    Settings::init();

    MainWindow window;
    window.show();

    if (Settings::getValue("workspace").toString().isEmpty()) {
        SelectWorkspace selectWorkspace(&window);
        selectWorkspace.exec();
    }

    return app.exec();
}
