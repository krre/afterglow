#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Core/Global.h"
#include "Core/Constants.h"
#include "NewCargoProject.h"
#include "Options.h"
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

void MainWindow::on_actionNewCargoProject_triggered() {
    NewCargoProject newCargoProject(this);
    newCargoProject.exec();
}

void MainWindow::on_actionNewRustFile_triggered() {
    qDebug() << "new rust";
}

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
}

void MainWindow::on_actionOptions_triggered() {
    Options options(this);
    options.exec();
}

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, tr("About %1").arg(APP_NAME),
        tr("<h3>%1 %2</h3> \
           Based on Qt %3<br> \
           Build on %4<br><br> \
           <a href=%5>%5</a><br><br>%6").
           arg(APP_NAME).arg(APP_VERSION).arg(QT_VERSION_STR).
           arg(__DATE__).arg(APP_URL).arg(APP_COPYRIGHT));
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

    ui->actionShowSidebar->setChecked(settings.value("showSidebar", true).toBool());
    ui->actionShowOutput->setChecked(settings.value("showOutput", true).toBool());

    settings.endGroup();
}

void MainWindow::writeSettings() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());

    settings.setValue("splitterMain", ui->splitterMain->saveState());
    settings.setValue("splitterSide", ui->splitterSide->saveState());

    settings.setValue("showSidebar", ui->actionShowSidebar->isChecked());
    settings.setValue("showOutput", ui->actionShowOutput->isChecked());

    settings.endGroup();
}
