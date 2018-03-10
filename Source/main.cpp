#include "UI/MainWindow.h"
#include "Core/Constants.h"
#include "Core/Global.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);

    Global::init();

    MainWindow window;
    window.show();

    return app.exec();
}
