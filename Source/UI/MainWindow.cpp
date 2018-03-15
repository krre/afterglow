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
#ifdef Q_OS_WIN
    #include <windows.h>
#endif
#include <QtWidgets>

MainWindow::MainWindow() :
        ui(new Ui::MainWindow) {
    ui->setupUi(this);

    cargoManager = new CargoManager(this);
    connect(cargoManager, &CargoManager::projectCreated, this, &MainWindow::onProjectCreated);
    connect(cargoManager, &CargoManager::outputMessage, this, &MainWindow::onOutputMessage);

    projectTreeView = new ProjectTreeView;
    connect(projectTreeView, &ProjectTreeView::openActivated, this, &MainWindow::addSourceTab);
    connect(projectTreeView, &ProjectTreeView::newFileActivated, this, &MainWindow::onFileCreated);
    connect(projectTreeView, &ProjectTreeView::removeActivated, this, &MainWindow::onFileRemoved);

    ui->tabWidgetSide->addTab(projectTreeView, tr("Project"));
    ui->tabWidgetSide->addTab(new QWidget, tr("Properties"));

    readSettings();
    updateMenuState();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    writeSettings();
    saveSession();
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

void MainWindow::on_actionSaveAll_triggered() {
    for (int i = 0; i < ui->tabWidgetSource->count(); i++) {
        Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->widget(i));
        editor->saveFile();
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
    projectTreeView->onNewRustFile();
}

void MainWindow::on_actionOpenFile_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), projectPath, "Rust (*.rs);;All Files(*.*)");
    if (!filePath.isEmpty()) {
        addSourceTab(filePath);
    }
}

void MainWindow::on_actionSave_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->saveFile();
}

void MainWindow::on_actionExit_triggered() {
    QApplication::quit();
}

void MainWindow::on_actionUndo_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->undo();
}

void MainWindow::on_actionRedo_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->redo();
}

void MainWindow::on_actionCut_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->cut();
}

void MainWindow::on_actionCopy_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->copy();
}

void MainWindow::on_actionPaste_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->paste();
}

void MainWindow::on_actionBuild_triggered() {
    on_actionSaveAll_triggered();
    cargoManager->build();
}

void MainWindow::on_actionRun_triggered() {
    on_actionSaveAll_triggered();
    cargoManager->run();
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
        Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->widget(index));
        editor->setFocus();
        QString filePath = editor->getFilePath();
        QModelIndex modelIndex = projectTreeView->getFsModel()->index(filePath);
        projectTreeView->setCurrentIndex(modelIndex);
        changeWindowTitle(filePath);
    } else {
        projectTreeView->setCurrentIndex(QModelIndex());
        changeWindowTitle();
    }

    updateMenuState();
}

void MainWindow::on_toolButtonCargoClear_clicked() {
    ui->textEditCargo->clear();
}

void MainWindow::on_toolButtonAppClear_clicked() {
    ui->textEditApplication->clear();
}

void MainWindow::onProjectCreated(const QString& path) {
    openProject(path);
}

void MainWindow::onOutputMessage(const QString& message) {
    int cargoTabIndex = static_cast<int>(OutputPane::Cargo);
    ui->tabWidgetOutput->setCurrentIndex(cargoTabIndex);
    ui->textEditCargo->append(message);
}

void MainWindow::onFileCreated(const QString& filePath) {
    addNewFile(filePath);
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
        connect(editor, &Editor::documentModified, this, &MainWindow::onDocumentModified);
        int index = ui->tabWidgetSource->addTab(editor, fi.fileName());
        ui->tabWidgetSource->setTabToolTip(index, filePath);
        ui->tabWidgetSource->setCurrentIndex(index);
    }
}

void MainWindow::addNewFile(const QString& filePath) {
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Failed to open session file for writing" << filePath;
            return;
        }
        file.write("");
        addSourceTab(filePath);
    }
}

void MainWindow::onDocumentModified(Editor* editor) {
    ui->tabWidgetSource->setTabText(ui->tabWidgetSource->indexOf(editor), editor->getModifiedName());
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

void MainWindow::saveSession() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    if (!settings.value("MainWindow/restoreSession").toBool() || projectPath.isEmpty()) {
        return;
    }

    QDir dir(projectPath);
    bool result = dir.mkdir(PROJECT_DATA_DIRECTORY);
#ifdef Q_OS_WIN
    // Set hidden attribute on created directory (need only for Windows).
    if (result) {
        QString directory = projectPath + "/" + PROJECT_DATA_DIRECTORY;
        wchar_t* charText = new wchar_t[directory.length() + 1];
        directory.toWCharArray(charText);
        charText[directory.length()] = 0; // append null terminator
        int attr = GetFileAttributes(charText);
        if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
            SetFileAttributes(charText, attr | FILE_ATTRIBUTE_HIDDEN);
         }
    }
#else
    Q_UNUSED(result)
#endif

    QString sessionPath = projectPath + "/" + PROJECT_DATA_DIRECTORY + "/" + PROJECT_SESSION_FILE;
    QFile saveFile(sessionPath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open session file for writing" << sessionPath;
        return;
    }

    QJsonArray openFiles;
    for (int i = 0; i < ui->tabWidgetSource->count(); i++) {
        Editor* cave = static_cast<Editor*>(ui->tabWidgetSource->widget(i));
        openFiles.append(QJsonValue(cave->getFilePath()));
    }

    QJsonObject obj;
    obj["openFiles"] = openFiles;
    obj["selectedTab"] = ui->tabWidgetSource->currentIndex();

    QJsonDocument saveDoc(obj);
    saveFile.write(saveDoc.toJson());
}

void MainWindow::restoreSession() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    if (!settings.value("MainWindow/restoreSession").toBool() || projectPath.isEmpty()) {
        return;
    }

    QString sessionPath = projectPath + "/" + PROJECT_DATA_DIRECTORY + "/" + PROJECT_SESSION_FILE;
    QFileInfo fi(sessionPath);
    if (!fi.exists()) {
        return;
    }

    QFile loadFile(sessionPath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open session file for reading" << sessionPath;
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonArray array = loadDoc.object()["openFiles"].toArray();
    int selectedTab = loadDoc.object()["selectedTab"].toInt();
    QString selectedFilePath = array.at(selectedTab).toString();

    for (int i = 0; i < array.count(); i++) {
        QString filePath = array.at(i).toString();
        QFileInfo fi(filePath);
        if (fi.exists()) {
            addSourceTab(filePath);
        }
    }

    int index = findSource(selectedFilePath);
    ui->tabWidgetSource->setCurrentIndex(index);
}

void MainWindow::openProject(const QString& path) {
    closeProject();
    projectTreeView->setRootPath(path);
    projectPath = path;
    cargoManager->setProjectPath(path);

    restoreSession();

    if (!ui->tabWidgetSource->count()) {
        changeWindowTitle();
    }

    updateMenuState();
}

void MainWindow::closeProject() {
    saveSession();
    on_actionCloseAll_triggered();
    projectTreeView->setRootPath(QString());
    projectPath = QString();
    changeWindowTitle();
    updateMenuState();
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

void MainWindow::updateMenuState() {
    ui->menuEdit->menuAction()->setVisible(!projectPath.isNull());
    ui->menuCargo->menuAction()->setVisible(!projectPath.isNull());

    int index = ui->tabWidgetSource->currentIndex();

    ui->actionSave->setEnabled(index >= 0);
    ui->actionSaveAs->setEnabled(index >= 0);
    ui->actionSaveAll->setEnabled(index >= 0);

    ui->actionClose->setEnabled(index >= 0);
    ui->actionCloseOther->setEnabled(index >= 0);
    ui->actionCloseAll->setEnabled(index >= 0);

    ui->menuEdit->menuAction()->setEnabled(index >= 0);
}
