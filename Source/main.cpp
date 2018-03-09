#include "UI/MainWindow.h"
#include "Constants.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName(QString(APP_NAME));
    app.setApplicationVersion(APP_VERSION);

    MainWindow window;
    window.show();

    return app.exec();
}
