#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Core/Global.h"
#include "Core/Constants.h"
#include <QtCore>
#include <QtWidgets>

MainWindow::MainWindow() :
        ui(new Ui::MainWindow) {
    ui->setupUi(this);

    readSettings();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::readSettings() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");

    if (!restoreGeometry(settings.value("geometry").toByteArray())) {
        resize(WINDOW_WIDTH, WINDOW_HEIGHT);
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
    }

    QVariant splitterMainSize = settings.value("splitterMain");
    if (splitterMainSize == QVariant()) {
        ui->splitterMain->setSizes({ 100, 500 });
    } else {
        ui->splitterMain->restoreState(splitterMainSize.toByteArray());
    }

    QVariant splitterSideSize = settings.value("splitterSide");
    if (splitterSideSize == QVariant()) {
        ui->splitterSide->setSizes({ 500, 100 });
    } else {
        ui->splitterSide->restoreState(splitterSideSize.toByteArray());
    }

    settings.endGroup();
}

void MainWindow::writeSettings() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitterMain", ui->splitterMain->saveState());
    settings.setValue("splitterSide", ui->splitterSide->saveState());
    settings.endGroup();
}
