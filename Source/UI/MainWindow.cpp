#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Core/Global.h"
#include "Core/Constants.h"
#include "NewProject.h"
#include "Options.h"
#include "Cargo/CargoManager.h"
#include "ProjectTreeView.h"
#include "ProjectProperties.h"
#include "Editor/Editor.h"
#include "NewName.h"
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
    connect(projectTreeView, &ProjectTreeView::renameActivated, this, &MainWindow::onFileRenamed);
    ui->tabWidgetSide->addTab(projectTreeView, tr("Project"));

    projectProperties = new ProjectProperties;
    ui->tabWidgetSide->addTab(projectProperties, tr("Properties"));

    readSettings();
    updateMenuState();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    writeSettings();
    saveSession();
    saveProjectProperties();
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionNewProject_triggered() {
    NewProject newCargoProject(this);
    newCargoProject.exec();

    if (!newCargoProject.getProjectPath().isEmpty()) {
        cargoManager->createProject(newCargoProject.getProjectTemplate(), newCargoProject.getProjectPath());
    }
}

void MainWindow::on_actionNewRustFile_triggered() {
    projectTreeView->onNewRustFile();
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

void MainWindow::on_actionClearMenuRecentFiles_triggered() {
    for (int i = ui->menuRecentFiles->actions().size() - Constants::SEPARATOR_AND_MENU_CLEAR_COUNT - 1; i >= 0; i--) {
        ui->menuRecentFiles->removeAction(ui->menuRecentFiles->actions().at(i));
    }

    updateMenuState();
}

void MainWindow::on_actionClearMenuRecentProjects_triggered() {
    for (int i = ui->menuRecentProjects->actions().size() - Constants::SEPARATOR_AND_MENU_CLEAR_COUNT - 1; i >= 0; i--) {
        ui->menuRecentProjects->removeAction(ui->menuRecentProjects->actions().at(i));
    }

    updateMenuState();
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
    projectTreeView->onNewFile();
}

void MainWindow::on_actionNewDirectory_triggered() {
    projectTreeView->onNewDirectory();
}

void MainWindow::on_actionOpen_triggered() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);
    QString workspace = settings.value("Path/workspace", Global::getDefaultWorkspacePath()).toString();
    QString dirPath = projectPath.isEmpty() ? workspace : projectPath;
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File or Project"), dirPath, "All Files(*.*)");
    if (filePath.isEmpty()) return;

    QFileInfo fi(filePath);
    if (fi.fileName() == "Cargo.toml") {
        QFileInfo fi(filePath);
        openProject(fi.absolutePath());
        addRecentProject(fi.absolutePath());
    } else {
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

void MainWindow::on_actionSelectAll_triggered() {
    Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->currentWidget());
    editor->selectAll();
}

void MainWindow::on_actionBuild_triggered() {
    on_actionSaveAll_triggered();
    cargoManager->build(projectProperties->getTarget());
}

void MainWindow::on_actionRun_triggered() {
    on_actionSaveAll_triggered();
    cargoManager->run(projectProperties->getTarget());
}

void MainWindow::on_actionOptions_triggered() {
    Options options(this);
    options.exec();
}

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, tr("About %1").arg(Constants::APP_NAME),
        tr("<h3>%1 %2</h3> \
           Based on Qt %3<br> \
           Build on %4<br><br> \
           <a href=%5>%5</a><br><br>%6")
            .arg(Constants::APP_NAME).arg(Constants::APP_VERSION).arg(QT_VERSION_STR)
            .arg(__DATE__).arg(Constants::APP_URL).arg(Constants::APP_COPYRIGHT));
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
    addRecentProject(path);
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
}

void MainWindow::onFileRenamed(const QString& oldPath, const QString& newPath) {
    int index = findSource(oldPath);
    if (index != -1) {
        Editor* editor = static_cast<Editor*>(ui->tabWidgetSource->widget(index));
        editor->setFilePath(newPath);
        onDocumentModified(editor);
    }
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

        addRecentFile(filePath);
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

void MainWindow::addRecentFile(const QString& filePath) {
    addRecentFileOrProject(ui->menuRecentFiles, filePath, [=] {
        addSourceTab(filePath);
    });
}

void MainWindow::addRecentProject(const QString& projectPath) {
    addRecentFileOrProject(ui->menuRecentProjects, projectPath, [=] {
        openProject(projectPath);
    });
}

void MainWindow::addRecentFileOrProject(QMenu* menu, const QString& filePath, const std::function<void()>& callback) {
    for (QAction* action : menu->actions()) {
        if (action->text() == filePath) {
            menu->removeAction(action);
        }
    }

    QAction* fileAction = new QAction(filePath);
    connect(fileAction, &QAction::triggered, callback);
    menu->insertAction(menu->actions().first(), fileAction);

    if (menu->actions().size() > Constants::MAX_RECENT_FILES + Constants::SEPARATOR_AND_MENU_CLEAR_COUNT) {
        menu->removeAction(menu->actions().at(menu->actions().size() - Constants::SEPARATOR_AND_MENU_CLEAR_COUNT - 1));
    }

    updateMenuState();
}

void MainWindow::saveProjectProperties() {
    QString path = projectPath + "/" + Constants::PROJECT_DATA_DIRECTORY + "/" + Constants::PROJECT_PROPERTIES_FILE;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open project properties file for writing" << path;
        return;
    }

    QJsonObject obj;
    obj["target"] = static_cast<int>(projectProperties->getTarget());

    QJsonDocument doc(obj);
    file.write(doc.toJson());
}

void MainWindow::loadProjectProperties() {
    QString path = projectPath + "/" + Constants::PROJECT_DATA_DIRECTORY + "/" + Constants::PROJECT_PROPERTIES_FILE;
    QFileInfo fi(path);
    if (!fi.exists()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open project properties file for reading" << path;
        return;
    }

    QJsonDocument doc(QJsonDocument::fromJson(file.readAll()));
    CargoManager::BuildTarget target = static_cast<CargoManager::BuildTarget>(doc.object()["target"].toInt());
    projectProperties->setTarget(target);
}

void MainWindow::readSettings() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");

    if (!restoreGeometry(settings.value("geometry").toByteArray())) {
        resize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
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

    ui->tabWidgetSide->setCurrentIndex(settings.value("sidebar", 0).toInt());

    QString lastProject = settings.value("lastProject", "").toString();
    if (!lastProject.isEmpty()) {
        openProject(lastProject);
        addRecentProject(lastProject);
    }

    int size = settings.beginReadArray("recentFiles");
    for (int i = size - 1; i >= 0; --i) {
        settings.setArrayIndex(i);
        QString filePath = settings.value("path").toString();
        addRecentFile(filePath);
    }
    settings.endArray();

    size = settings.beginReadArray("recentProjects");
    for (int i = size - 1; i >= 0; --i) {
        settings.setArrayIndex(i);
        QString projectPath = settings.value("path").toString();
        addRecentProject(projectPath);
    }
    settings.endArray();

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

    settings.setValue("sidebar", ui->tabWidgetSide->currentIndex());

    settings.setValue("lastProject", projectPath);

    settings.beginWriteArray("recentFiles");
    for (int i = 0; i < ui->menuRecentFiles->actions().size() - Constants::SEPARATOR_AND_MENU_CLEAR_COUNT; ++i) {
        settings.setArrayIndex(i);
        settings.setValue("path", ui->menuRecentFiles->actions().at(i)->text());
    }
    settings.endArray();

    settings.beginWriteArray("recentProjects");
    for (int i = 0; i < ui->menuRecentProjects->actions().size() - Constants::SEPARATOR_AND_MENU_CLEAR_COUNT; ++i) {
        settings.setArrayIndex(i);
        settings.setValue("path", ui->menuRecentProjects->actions().at(i)->text());
    }
    settings.endArray();

    settings.endGroup();
}

void MainWindow::saveSession() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    if (!settings.value("MainWindow/restoreSession").toBool() || projectPath.isEmpty()) {
        return;
    }

    QDir dir(projectPath);
    bool result = dir.mkdir(Constants::PROJECT_DATA_DIRECTORY);
#ifdef Q_OS_WIN
    // Set hidden attribute on created directory (need only for Windows).
    if (result) {
        QString directory = projectPath + "/" + Constants::PROJECT_DATA_DIRECTORY;
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

    QString path = projectPath + "/" + Constants::PROJECT_DATA_DIRECTORY + "/" + Constants::PROJECT_SESSION_FILE;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open session file for writing" << path;
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

    QJsonDocument doc(obj);
    file.write(doc.toJson());
}

void MainWindow::restoreSession() {
    QSettings settings(Global::getPortableSettingsPath(), QSettings::IniFormat);

    if (!settings.value("MainWindow/restoreSession").toBool() || projectPath.isEmpty()) {
        return;
    }

    QString path = projectPath + "/" + Constants::PROJECT_DATA_DIRECTORY + "/" + Constants::PROJECT_SESSION_FILE;
    QFileInfo fi(path);
    if (!fi.exists()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open session file for reading" << path;
        return;
    }

    QJsonDocument doc(QJsonDocument::fromJson(file.readAll()));
    QJsonArray array = doc.object()["openFiles"].toArray();
    int selectedTab = doc.object()["selectedTab"].toInt();
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
    projectPath = path;
    projectTreeView->setRootPath(path);
    cargoManager->setProjectPath(path);

    loadProjectProperties();
    restoreSession();

    if (!ui->tabWidgetSource->count()) {
        changeWindowTitle();
    }

    updateMenuState();
}

void MainWindow::closeProject() {
    if (projectPath.isNull()) return;

    saveSession();
    saveProjectProperties();
    projectProperties->reset();

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

    ui->menuRecentProjects->menuAction()->setEnabled(ui->menuRecentProjects->actions().size() > Constants::SEPARATOR_AND_MENU_CLEAR_COUNT);
    ui->menuRecentFiles->menuAction()->setEnabled(ui->menuRecentFiles->actions().size() > Constants::SEPARATOR_AND_MENU_CLEAR_COUNT);

    ui->menuEdit->menuAction()->setEnabled(index >= 0);
}
