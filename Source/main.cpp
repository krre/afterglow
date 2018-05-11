#include "UI/MainWindow.h"
#include "UI/SelectWorkspace.h"
#include "Core/Constants.h"
#include "Core/Settings.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName(Constants::APP_NAME);
    app.setApplicationVersion(Constants::APP_VERSION);

    Settings::init();

    MainWindow window;
    window.show();

    if (Settings::getValue("workspace").toString().isEmpty()) {
        SelectWorkspace selectWorkspace(&window);
        selectWorkspace.exec();
    }

    return app.exec();
}
