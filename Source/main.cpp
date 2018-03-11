#include "UI/MainWindow.h"
#include "UI/SelectWorkspace.h"
#include "Core/Constants.h"
#include "Core/Global.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);

    Global::init();

    MainWindow window;
    window.show();

    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    if (settings.value("Path/workspace") == QVariant()) {
        SelectWorkspace selectWorkspace(&window);
        selectWorkspace.exec();
    }

    return app.exec();
}
