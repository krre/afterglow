#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Core/Global.h"
#include "Core/Constants.h"
#include "NewProject.h"
#include "Options.h"
#include "Cargo/CargoManager.h"
#include "ProjectTreeView.h"
#include "Editor/Editor.h"
#include "NewFile.h"
#include <QtWidgets>

MainWindow::MainWindow() :
        ui(new Ui::MainWindow) {
    ui->setupUi(this);

    cargoManager = new CargoManager(this);
    connect(cargoManager, &CargoManager::projectCreated, this, &MainWindow::onProjectCreated);
    connect(cargoManager, &CargoManager::outputMessage, this, &MainWindow::onOutputMessage);

    projectTreeView = new ProjectTreeView;
    connect(projectTreeView, &ProjectTreeView::openActivated, this, &MainWindow::addSourceTab);
    connect(projectTreeView, &ProjectTreeView::removeActivated, this, &MainWindow::onFileRemoved);

    ui->tabWidgetSide->addTab(projectTreeView, tr("Project"));
    ui->tabWidgetSide->addTab(new QWidget, tr("Properties"));

    readSettings();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    writeSettings();
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionNewProject_triggered() {
    NewProject newCargoProject(this);
    newCargoProject.exec();

    if (!newCargoProject.getProjectPath().isEmpty()) {
        cargoManager->createProject(newCargoProject.getProjectTemplate(), newCargoProject.getProjectPath());
    }
}

void MainWindow::on_actionOpenProject_triggered() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    QString workspace = settings.value("Path/workspace", Global::getDefaultWorkspacePath()).toString();
    QString dirPath = QFileDialog::getExistingDirectory(this, QString(), workspace);
    if (!dirPath.isEmpty()) {
        openProject(dirPath);
    }
}

void MainWindow::on_actionCloseProject_triggered() {
    closeProject();
}

void MainWindow::on_actionSaveAs_triggered() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), projectPath, "Rust (*.rs);;All Files(*.*)");
    if (!filePath.isEmpty()) {
        // TODO: Duplicate file
        addSourceTab(filePath);
    }
}

void MainWindow::on_actionClose_triggered() {
    on_tabWidgetSource_tabCloseRequested(ui->tabWidgetSource->currentIndex());
}

void MainWindow::on_actionCloseAll_triggered() {
    while (ui->tabWidgetSource->count()) {
        on_tabWidgetSource_tabCloseRequested(0);
    }
}

void MainWindow::on_actionCloseOther_triggered() {
    int i = 0;
    while (ui->tabWidgetSource->count() > 1) {
        if (i != ui->tabWidgetSource->currentIndex()) {
            on_tabWidgetSource_tabCloseRequested(i);
        } else {
            i++;
        }
    }
}

void MainWindow::on_actionNewFile_triggered() {
    NewFile newFile(projectPath);
    newFile.exec();
    if (!newFile.getFilePath().isEmpty()) {
        // TODO: Create new file
        addSourceTab(newFile.getFilePath());
    }
}

void MainWindow::on_actionOpenFile_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), projectPath, "Rust (*.rs);;All Files(*.*)");
    if (!filePath.isEmpty()) {
        addSourceTab(filePath);
    }
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
           <a href=%5>%5</a><br><br>%6")
            .arg(APP_NAME).arg(APP_VERSION).arg(QT_VERSION_STR)
            .arg(__DATE__).arg(APP_URL).arg(APP_COPYRIGHT));
}

void MainWindow::on_tabWidgetSource_tabCloseRequested(int index) {
    QWidget* widget = ui->tabWidgetSource->widget(index);
    ui->tabWidgetSource->removeTab(index);
    delete widget;
}

void MainWindow::on_tabWidgetSource_currentChanged(int index) {
    if (index >= 0) {
        QString filePath = static_cast<Editor*>(ui->tabWidgetSource->widget(index))->getFilePath();
        QModelIndex modelIndex = projectTreeView->getFsModel()->index(filePath);
        projectTreeView->setCurrentIndex(modelIndex);
        changeWindowTitle(filePath);
    } else {
        projectTreeView->setCurrentIndex(QModelIndex());
        changeWindowTitle();
    }

    ui->actionSaveAs->setEnabled(index >= 0);
    ui->actionClose->setEnabled(index >= 0);
    ui->actionCloseOther->setEnabled(index >= 0);
    ui->actionCloseAll->setEnabled(index >= 0);
}

void MainWindow::onProjectCreated(const QString& path) {
    openProject(path);
}

void MainWindow::onOutputMessage(const QString& message) {
    int cargoTabIndex = static_cast<int>(OutputPane::Cargo);
    ui->tabWidgetOutput->setCurrentIndex(cargoTabIndex);
    ui->textEditCargo->append(message);
}

void MainWindow::onFileRemoved(const QString& filePath) {
    on_tabWidgetSource_tabCloseRequested(findSource(filePath));
    QDir dir;
    dir.remove(filePath);
}

void MainWindow::addSourceTab(const QString& filePath) {
    int tabIndex = findSource(filePath);
    if (tabIndex != -1) {
        ui->tabWidgetSource->setCurrentIndex(tabIndex);
    } else {
        QFileInfo fi(filePath);
        Editor* editor = new Editor(filePath);
        int index = ui->tabWidgetSource->addTab(editor, fi.fileName());
        ui->tabWidgetSource->setTabToolTip(index, filePath);
        ui->tabWidgetSource->setCurrentIndex(index);
    }
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

    QString lastProject = settings.value("lastProject", "").toString();
    if (!lastProject.isEmpty()) {
        openProject(lastProject);
    }

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

    settings.setValue("lastProject", projectPath);

    settings.endGroup();
}

void MainWindow::openProject(const QString& path) {
    closeProject();
    projectTreeView->setRootPath(path);
    projectPath = path;

    if (!ui->tabWidgetSource->count()) {
        changeWindowTitle();
    }
}

void MainWindow::closeProject() {
    projectTreeView->setRootPath(QString());
    projectPath = QString();
    changeWindowTitle();
}

void MainWindow::changeWindowTitle(const QString& filePath) {
    QString title = QApplication::applicationName();

    if (!projectPath.isEmpty()) {
        QFileInfo fi(projectPath);
        title = fi.baseName() + " - " + title;
    }

    if (!filePath.isEmpty()) {
        QFileInfo fi(filePath);
        title = fi.fileName() + " - " + title;
    }

    setWindowTitle(title);
}

int MainWindow::findSource(const QString& filePath) {
    for (int i = 0; i < ui->tabWidgetSource->count(); i++) {
        Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->widget(i));
        if (editor->getFilePath() == filePath) {
            return i;
        }
    }

    return -1;
}
