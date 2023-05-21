#include "MainWindow.h"
#include "NewProject.h"
#include "GoToLine.h"
#include "Preferences.h"
#include "ProjectTree.h"
#include "ProjectProperties.h"
#include "IssueListView.h"
#include "IssueModel.h"
#include "ActionManager.h"
#include "installer/RustInstaller.h"
#include "process/CargoManager.h"
#include "texteditor/TextEditor.h"
#include "texteditor/AutoCompleter.h"
#include "texteditor/SyntaxHighlightManager.h"
#include "core/Global.h"
#include "core/Constants.h"
#include "core/Settings.h"
#include "core/Utils.h"
#include "process/RlsManager.h"
#ifdef Q_OS_WIN
    #include <windows.h>
#endif
#include <QtWidgets>

MainWindow::MainWindow() {
    new SyntaxHighlightManager(this);
    new ActionManager(this);

    resize(1040, 620);

    mainSplitter = new QSplitter;
    mainSplitter->setOrientation(Qt::Horizontal);
    mainSplitter->setHandleWidth(0);
    mainSplitter->setChildrenCollapsible(false);

    setCentralWidget(mainSplitter);

    sideTabWidget = new QTabWidget(mainSplitter);
    sideTabWidget->setMinimumSize(QSize(50, 0));
    sideTabWidget->setBaseSize(QSize(200, 0));
    sideTabWidget->setTabPosition(QTabWidget::West);
    mainSplitter->addWidget(sideTabWidget);

    sideSplitter = new QSplitter(mainSplitter);
    sideSplitter->setOrientation(Qt::Vertical);
    sideSplitter->setHandleWidth(0);
    sideSplitter->setChildrenCollapsible(false);

    sourceTabWidget = new QTabWidget(sideSplitter);
    sourceTabWidget->setMinimumSize(QSize(0, 50));
    sourceTabWidget->setTabsClosable(true);
    sourceTabWidget->setMovable(true);
    sideSplitter->addWidget(sourceTabWidget);

    connect(sourceTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onSourceTabClose);
    connect(sourceTabWidget, &QTabWidget::tabBarClicked, this, &MainWindow::onSourceTabCurrentChanged);

    outputTabWidget = new QTabWidget(sideSplitter);
    outputTabWidget->setMinimumSize(QSize(0, 50));

    QWidget* cargoTab = new QWidget();

    QHBoxLayout* cargoHorizontalLayout = new QHBoxLayout(cargoTab);
    cargoHorizontalLayout->setSpacing(0);
    cargoHorizontalLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* cargoVerticalLayout = new QVBoxLayout();
    cargoVerticalLayout->setSpacing(0);
    cargoVerticalLayout->setContentsMargins(1, 1, 1, 1);

    QFont font = Global::fontAwesomeFont();

    auto buildToolButtonCargo = new QToolButton();
    buildToolButtonCargo->setToolTip(tr("Build"));
    buildToolButtonCargo->setFont(font);
    buildToolButtonCargo->setText(Const::FontAwesome::Cog);
    cargoVerticalLayout->addWidget(buildToolButtonCargo);

    auto runToolButtonCargo = new QToolButton(cargoTab);
    runToolButtonCargo->setToolTip(tr("Run"));
    runToolButtonCargo->setFont(font);
    runToolButtonCargo->setText(Const::FontAwesome::Play);
    cargoVerticalLayout->addWidget(runToolButtonCargo);

    auto stopToolButtonCargo = new QToolButton(cargoTab);
    stopToolButtonCargo->setToolTip(tr("Stop"));
    stopToolButtonCargo->setFont(font);
    stopToolButtonCargo->setText(Const::FontAwesome::Stop);
    cargoVerticalLayout->addWidget(stopToolButtonCargo);

    auto clearToolButtonCargo = new QToolButton(cargoTab);
    clearToolButtonCargo->setToolTip(tr("Clear"));
    clearToolButtonCargo->setFont(font);
    clearToolButtonCargo->setText(Const::FontAwesome::TrashAlt);
    cargoVerticalLayout->addWidget(clearToolButtonCargo);

    cargoVerticalLayout->addStretch();

    connect(buildToolButtonCargo, &QToolButton::clicked, this, &MainWindow::onCargoBuild);
    connect(runToolButtonCargo, &QToolButton::clicked, this, &MainWindow::onCargoRun);
    connect(stopToolButtonCargo, &QToolButton::clicked, this, &MainWindow::onCargoStop);
    connect(clearToolButtonCargo, &QToolButton::clicked, this, &MainWindow::onCargoClear);

    cargoHorizontalLayout->addLayout(cargoVerticalLayout);

    cargoPlainTextEdit = new QPlainTextEdit(cargoTab);
    cargoPlainTextEdit->setFocusPolicy(Qt::StrongFocus);
    cargoPlainTextEdit->setReadOnly(true);

    cargoHorizontalLayout->addWidget(cargoPlainTextEdit);

    outputTabWidget->addTab(cargoTab, tr("Cargo"));

    QWidget* issuesTab = new QWidget();
    auto issueHorizontalLayout = new QHBoxLayout(issuesTab);
    issueHorizontalLayout->setSpacing(0);
    issueHorizontalLayout->setContentsMargins(0, 0, 0, 0);

    auto issueVerticalLayout = new QVBoxLayout();
    issueVerticalLayout->setSpacing(0);

    auto clearToolButtonIssues = new QToolButton(issuesTab);
    clearToolButtonIssues->setToolTip(tr("Clear"));
    clearToolButtonIssues->setFont(font);
    clearToolButtonIssues->setText(Const::FontAwesome::TrashAlt);
    issueVerticalLayout->addWidget(clearToolButtonIssues);

    issueVerticalLayout->addStretch();

    connect(clearToolButtonIssues, &QToolButton::clicked, this, &MainWindow::onIssuesClear);

    issueHorizontalLayout->addLayout(issueVerticalLayout);

    auto issueWidget = new QWidget(issuesTab);
    issueHorizontalLayout->addWidget(issueWidget);

    outputTabWidget->addTab(issuesTab, tr("Issues"));

    sideSplitter->addWidget(outputTabWidget);
    mainSplitter->addWidget(sideSplitter);

    sideTabWidget->setCurrentIndex(-1);
    outputTabWidget->setCurrentIndex(0);

    projectProperties = new ProjectProperties;

    cargoManager = new CargoManager(projectProperties, this);
    connect(cargoManager, &CargoManager::projectCreated, this, &MainWindow::onProjectCreated);
    connect(cargoManager, &CargoManager::consoleMessage, this, &MainWindow::onCargoMessage);

    projectTree = new ProjectTree;
    connect(projectTree, &ProjectTree::openActivated, this, &MainWindow::addSourceTab);
    connect(projectTree, &ProjectTree::newFileActivated, this, &MainWindow::onFileCreated);
    connect(projectTree, &ProjectTree::removeActivated, this, &MainWindow::onFileRemoved);
    connect(projectTree, &ProjectTree::renameActivated, this, &MainWindow::onFileRenamed);

    sideTabWidget->addTab(projectTree, tr("Project"));
    sideTabWidget->addTab(projectProperties, tr("Properties"));

    issueModel = new IssueModel(this);
    connect(issueModel, &IssueModel::countChanged, this, &MainWindow::onIssueCountChanged);
    issueListView = new IssueListView(issueModel);
    connect(issueListView, &IssueListView::doubleClicked, [this] (const QModelIndex& index) {
        QString filename = issueModel->data(index, static_cast<int>(IssueModel::Role::Filename)).toString();
        if (!filename.isEmpty()) {
            QString line = issueModel->data(index, static_cast<int>(IssueModel::Role::Line)).toString();
            addSourceTab(projectPath + "/" + filename);
            editor->goToLine(line.toInt());
        }
    });

    issueHorizontalLayout->addWidget(issueListView);

    new RlsManager(this);
    RlsManager::setShowDebug(Settings::value("debug.dump.rlsMessages").toBool());
//    RlsManager::start();

    completer = new AutoCompleter(this);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);

    createActions();
    loadSettings();
    updateMenuState();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    saveSettings();
    closeProject();
    event->accept();
}

void MainWindow::onNewProjectAction() {
    NewProject newCargoProject(this);
    if (newCargoProject.exec() == QDialog::Rejected) return;
    cargoManager->createProject(newCargoProject.projectTemplate(), newCargoProject.path());
}

void MainWindow::onNewRustFileAction() {
    projectTree->onNewRustFile();
}

void MainWindow::onCloseProjectAction() {
    closeProject();
}

void MainWindow::onSaveAsAction() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), editor->getFilePath(), "Rust (*.rs);;All Files(*.*)");
    if (!filePath.isEmpty()) {
        bool result = QFile::copy(editor->getFilePath(), filePath);
        if (result) {
            addSourceTab(filePath);
        } else {
            QMessageBox::critical(this, tr("Duplicate"), tr("Error creating copy of file"));
        }
    }
}

void MainWindow::onSaveAllAction() {
    for (int i = 0; i < sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(sourceTabWidget->widget(i));
        editor->saveFile();
    }
}

void MainWindow::onClearMenuRecentFilesAction() {
    for (int i = recentFilesMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount - 1; i >= 0; i--) {
        recentFilesMenu->removeAction(recentFilesMenu->actions().at(i));
    }

    updateMenuState();
}

void MainWindow::onClearMenuRecentProjectsAction() {
    for (int i = recentProjectsMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount - 1; i >= 0; i--) {
        recentProjectsMenu->removeAction(recentProjectsMenu->actions().at(i));
    }

    updateMenuState();
}

void MainWindow::onCloseAction() {
    onSourceTabClose(sourceTabWidget->currentIndex());
}

void MainWindow::onCloseAllAction() {
    while (sourceTabWidget->count()) {
        onSourceTabClose(0);
    }
}

void MainWindow::onCloseOtherAction() {
    int i = 0;

    while (sourceTabWidget->count() > 1) {
        if (i != sourceTabWidget->currentIndex()) {
            onSourceTabClose(i);
        } else {
            i++;
        }
    }
}

void MainWindow::onNewFileAction() {
    projectTree->onNewFile();
}

void MainWindow::onNewDirectoryAction() {
    projectTree->onNewDirectory();
}

void MainWindow::onOpenAction() {
    QString dirPath = projectPath.isEmpty() ? Global::workspacePath() : projectPath;
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File or Project"), dirPath, "All Files(*.*)");
    if (filePath.isEmpty()) return;

    QFileInfo fi(filePath);
    if (fi.fileName() == "Cargo.toml") {
        QFileInfo fi(filePath);
        openProject(fi.absolutePath());
    } else {
        addSourceTab(filePath);
    }
}

void MainWindow::onSaveAction() {
    editor->saveFile();
}

void MainWindow::onUndoAction() {
    editor->undo();
}

void MainWindow::onRedoAction() {
    editor->redo();
}

void MainWindow::onCutAction() {
    editor->cut();
}

void MainWindow::onCopyAction() {
    editor->copy();
}

void MainWindow::onPasteAction() {
    editor->paste();
}

void MainWindow::onSelectAllAction() {
    editor->selectAll();
}

void MainWindow::onToggleSingleLineCommentAction() {
    editor->toggleSingleLineComment();
}

void MainWindow::onToggleBlockCommentAction() {
    editor->toggleBlockComment();
}

void MainWindow::onAutoCompleterAction() {
    editor->openAutoCompleter();
}

void MainWindow::onJoinLinesAction() {
    editor->joinLines();
}

void MainWindow::onDuplicateLineAction() {
    editor->duplicateLine();
}

void MainWindow::onCutLineAction() {
    editor->cutLine();
}

void MainWindow::onIncreaseIndentAction() {
    editor->increaseIndent();
}

void MainWindow::onDecreaseIndentAction() {
    editor->decreaseIndent();
}

void MainWindow::onGoToLineAction() {
    GoToLine goToLine(this);
    if (goToLine.exec() == QDialog::Rejected) return;
    editor->goToLine(goToLine.line());
}

void MainWindow::onCleanTrailingWhitespaceAction() {
    editor->cleanTrailingWhitespace();
}

void MainWindow::onBuildAction() {
    prepareBuild();
    cargoManager->build();
}

void MainWindow::onRunAction() {
    prepareBuild();
    cargoManager->run();
}

void MainWindow::onCheckAction() {
    prepareBuild();
    cargoManager->check();
}

void MainWindow::onStopAction() {
    cargoManager->stop();
}

void MainWindow::onCleanAction() {
    cargoManager->clean();
}

void MainWindow::onRustInstallerAction() {
    RustInstaller installer(this);
    installer.exec();
}

void MainWindow::onPreferencesAction() {
    Preferences preferences(this);
    connect(&preferences, &Preferences::openPrefs, this, [=, this] {
        addSourceTab(Settings::prefsPath());
    });
    preferences.exec();
}

void MainWindow::onDocumentationAction() {
    Utils::runRustupCommand({ "doc" });
}

void MainWindow::onStandardLibraryAction() {
    Utils::runRustupCommand({"doc", "--std" });
}

void MainWindow::onTheBookAction() {
    Utils::runRustupCommand({ "doc", "--book" });
}

void MainWindow::onAboutAction() {
    QMessageBox::about(this, tr("About %1").arg(Const::App::Name),
        tr("<h3>%1 %2 %3</h3>\
           IDE for Rust programming language<br><br> \
           Based on Qt %4<br> \
           Build on %5<br><br> \
           <a href=%6>%6</a><br><br>%7")
            .arg(Const::App::Name, Const::App::Version, Const::App::Status,
                 QT_VERSION_STR, __DATE__, Const::App::Url, Const::App::Copyright));
}

void MainWindow::onSourceTabClose(int index) {
    QWidget* widget = sourceTabWidget->widget(index);
    sourceTabWidget->removeTab(index);
    delete widget;
}

void MainWindow::onSourceTabCurrentChanged(int index) {
    if (index >= 0) {
        editor = static_cast<TextEditor*>(sourceTabWidget->widget(index));
        editor->setAutoCompleter(completer);
        editor->setFocus();
        QString filePath = editor->getFilePath();
        projectTree->selectFile(filePath);
        changeWindowTitle(filePath);
    } else {
        editor = nullptr;
        projectTree->setCurrentIndex(QModelIndex());
        changeWindowTitle();
    }

    updateMenuState();
}

void MainWindow::onCargoBuild() {
    onBuildAction();
}

void MainWindow::onCargoRun() {
    onRunAction();
}

void MainWindow::onCargoClear() {
    cargoPlainTextEdit->clear();
}

void MainWindow::onCargoStop() {
    cargoManager->stop();
}

void MainWindow::onIssuesClear() {
    issueListView->setCurrentIndex(QModelIndex());
    issueModel->clear();
}

void MainWindow::onProjectCreated(const QString& path) {
    openProject(path, true);
}

void MainWindow::onCargoMessage(const QString& message, bool html, bool start) {
    if (start) {
        cargoPlainTextEdit->clear();
    }

    bool isJson = false;
    const auto blocks = message.split('\n');

    for (const QString& block : blocks) {
        if (block.isEmpty()) continue;

        if (block.at(0) == '{') {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(block.toUtf8(), &error);

            if (!doc.isNull() && doc.isObject()) {
                isJson = true;
                QJsonObject obj = doc.object();
                if (obj.contains("reason")) {
                    if (obj["reason"].toString() == "compiler-message") {
                        issueModel->appendMessage(obj);
                    }

                    if (Settings::value("debug.dump.compilerMessages").toBool()) {
                        qDebug() << block;
                    }
                }
            }
        }
    }

    if (isJson) return;

    if (html) {
        cargoPlainTextEdit->appendHtml(message);
    } else {
        cargoPlainTextEdit->insertPlainText(message);
    }

    cargoPlainTextEdit->verticalScrollBar()->setValue(cargoPlainTextEdit->verticalScrollBar()->maximum());
}

void MainWindow::onFileCreated(const QString& filePath) {
    addNewFile(filePath);
}

void MainWindow::onFileRemoved(const QString& filePath) {
    QVector<int> indices;
    for (int i = 0; i < sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(sourceTabWidget->widget(i));
        if (editor->getFilePath().contains(filePath)) {
            indices.append(i);
        }
    }

    // Remove tabs in reverse order
    for (int i = indices.count() - 1; i >= 0; i--) {
        onSourceTabClose(indices.at(i));
    }
}

void MainWindow::onFileRenamed(const QString& oldPath, const QString& newPath) {
    for (int i = 0; i < sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(sourceTabWidget->widget(i));
        if (editor->getFilePath().contains(oldPath)) {
            QFileInfo fi(newPath);
            if (fi.isDir()) {
                editor->setFilePath(editor->getFilePath().replace(oldPath, newPath));
            } else {
                editor->setFilePath(newPath);
                onDocumentModified(editor);
            }
        }
    }
}

int MainWindow::addSourceTab(const QString& filePath) {
    int tabIndex = findSource(filePath);
    if (tabIndex != -1) {
        sourceTabWidget->setCurrentIndex(tabIndex);
        return tabIndex;
    } else {
        QFileInfo fi(filePath);
        editor = new TextEditor(filePath);
        connect(editor, &TextEditor::documentModified, this, &MainWindow::onDocumentModified);
        int index = sourceTabWidget->addTab(editor, fi.fileName());
        sourceTabWidget->setTabToolTip(index, filePath);
        sourceTabWidget->setCurrentIndex(index);

        addRecentFile(filePath);

        return index;
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

void MainWindow::onDocumentModified(TextEditor* editor) {
    sourceTabWidget->setTabText(sourceTabWidget->indexOf(editor), editor->getModifiedName());
}

void MainWindow::onIssueCountChanged(int count) {
    QString title = tr("Issues");
    if (count) {
        title += QString(" (%1)").arg(count);
    }

    outputTabWidget->setTabText(static_cast<int>(OutputPane::Issues), title);
}

void MainWindow::createActions() {
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    QMenu* newFileMenu = fileMenu->addMenu(tr("New"));

    ActionManager::addAction(Const::Action::NewProject, newFileMenu->addAction(tr("Project..."), this, &MainWindow::onNewProjectAction));
    ActionManager::addAction(Const::Action::NewRustFile, newFileMenu->addAction(tr("Rust File..."), Qt::CTRL | Qt::Key_N, this, &MainWindow::onNewRustFileAction));
    ActionManager::addAction(Const::Action::NewFile, newFileMenu->addAction(tr("File..."), this, &MainWindow::onNewFileAction));
    ActionManager::addAction(Const::Action::NewDirectory, newFileMenu->addAction(tr("Directory..."), this, &MainWindow::onNewDirectoryAction));

    ActionManager::addAction(Const::Action::Open, fileMenu->addAction(tr("Open..."), Qt::CTRL | Qt::Key_O, this, &MainWindow::onOpenAction));
    ActionManager::addAction(Const::Action::CloseProject, fileMenu->addAction(tr("Close Project"), this, &MainWindow::onCloseProjectAction));

    recentFilesMenu = fileMenu->addMenu(tr("Recent Files"));
    recentFilesMenu->addSeparator();
    recentFilesMenu->addAction(tr("Clear Menu"), this, &MainWindow::onClearMenuRecentFilesAction);

    recentProjectsMenu = fileMenu->addMenu(tr("Recent Projects"));
    recentProjectsMenu->addSeparator();
    recentProjectsMenu->addAction(tr("Clear Menu"), this, &MainWindow::onClearMenuRecentProjectsAction);

    fileMenu->addSeparator();

    ActionManager::addAction(Const::Action::Save, fileMenu->addAction(tr("Save"), Qt::CTRL | Qt::Key_S, this, &MainWindow::onSaveAction));
    ActionManager::addAction(Const::Action::SaveAs, fileMenu->addAction(tr("Save As..."), this, &MainWindow::onSaveAsAction));
    ActionManager::addAction(Const::Action::SaveAll, fileMenu->addAction(tr("Save All"), Qt::CTRL | Qt::SHIFT | Qt::Key_S, this, &MainWindow::onSaveAllAction));

    fileMenu->addSeparator();

    ActionManager::addAction(Const::Action::Close, fileMenu->addAction(tr("Close"), Qt::CTRL | Qt::Key_W, this, &MainWindow::onCloseAction));
    ActionManager::addAction(Const::Action::CloseAll, fileMenu->addAction(tr("Close All"), Qt::CTRL | Qt::SHIFT | Qt::Key_W, this, &MainWindow::onCloseAllAction));
    ActionManager::addAction(Const::Action::CloseOther, fileMenu->addAction(tr("Close Other"), this, &MainWindow::onCloseOtherAction));

    fileMenu->addSeparator();

    fileMenu->addAction(tr("Preferences..."), this, &MainWindow::onPreferencesAction);

    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"), Qt::CTRL | Qt::Key_Q, this, &MainWindow::close);

    editMenu = menuBar()->addMenu(tr("Edit"));
    editMenu->addAction(tr("Undo"), Qt::CTRL | Qt::Key_Z, this, &MainWindow::onUndoAction);
    editMenu->addAction(tr("Redo"), Qt::CTRL | Qt::SHIFT | Qt::Key_N, this, &MainWindow::onRedoAction);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cut"), Qt::CTRL | Qt::Key_X, this, &MainWindow::onCutAction);
    editMenu->addAction(tr("Copy"), Qt::CTRL | Qt::Key_C, this, &MainWindow::onCopyAction);
    editMenu->addAction(tr("Paste"), Qt::CTRL | Qt::Key_V, this, &MainWindow::onPasteAction);
    editMenu->addSeparator();
    editMenu->addAction(tr("Select All"), Qt::CTRL | Qt::Key_A, this, &MainWindow::onSelectAllAction);
    editMenu->addSeparator();

    QMenu* lineMenu = editMenu->addMenu(tr("Line"));
    lineMenu->addAction(tr("Duplicate Line"), Qt::CTRL | Qt::Key_D, this, &MainWindow::onDuplicateLineAction);
    lineMenu->addAction(tr("Join Lines"), Qt::CTRL | Qt::Key_J, this, &MainWindow::onJoinLinesAction);
    lineMenu->addAction(tr("Cut Line"), Qt::CTRL | Qt::Key_Delete, this, &MainWindow::onCutLineAction);

    QMenu* indentMenu = editMenu->addMenu(tr("Indent"));
    indentMenu->addAction(tr("Increase Indent"), Qt::Key_Tab, this, &MainWindow::onIncreaseIndentAction);
    indentMenu->addAction(tr("Decrease Indent"), Qt::SHIFT | Qt::Key_Tab, this, &MainWindow::onDecreaseIndentAction);

    QMenu* commentMenu = editMenu->addMenu(tr("Comment"));
    commentMenu->addAction(tr("Toggle Single Line Comment"), Qt::CTRL | Qt::Key_Slash, this, &MainWindow::onToggleSingleLineCommentAction);
    commentMenu->addAction(tr("Toggle Block Comment"), Qt::CTRL | Qt::SHIFT | Qt::Key_Slash, this, &MainWindow::onToggleBlockCommentAction);

    editMenu->addAction(tr("Auto-Completer"), Qt::CTRL | Qt::Key_Space, this, &MainWindow::onAutoCompleterAction);
    editMenu->addAction(tr("Clean Trailing Whitespace"), this, &MainWindow::onCleanTrailingWhitespaceAction);
    editMenu->addSeparator();
    editMenu->addAction(tr("Go to Line..."), Qt::CTRL | Qt::Key_G, this, &MainWindow::onGoToLineAction);

    buildMenu = menuBar()->addMenu(tr("Build"));
    ActionManager::addAction(Const::Action::Build, buildMenu->addAction(tr("Build"), Qt::CTRL | Qt::Key_B, this, &MainWindow::onBuildAction));
    ActionManager::addAction(Const::Action::Run, buildMenu->addAction(tr("Run"), Qt::CTRL | Qt::Key_R, this, &MainWindow::onRunAction));
    ActionManager::addAction(Const::Action::Check, buildMenu->addAction(tr("Check"), Qt::CTRL | Qt::Key_K, this, &MainWindow::onCheckAction));
    ActionManager::addAction(Const::Action::Stop, buildMenu->addAction(tr("Stop"), this, &MainWindow::onStopAction));
    ActionManager::addAction(Const::Action::Clean, buildMenu->addAction(tr("Clean"), this, &MainWindow::onCleanAction));

    QMenu* toolsMenu = menuBar()->addMenu(tr("Tools"));
    toolsMenu->addAction(tr("Rust Installer..."), this, &MainWindow::onRustInstallerAction);

    QMenu* viewMenu = menuBar()->addMenu(tr("View"));

    QAction* showSidebarAction = viewMenu->addAction(tr("Show Sidebar"), sideTabWidget, &QTableWidget::setVisible);
    showSidebarAction->setCheckable(true);
    showSidebarAction->setChecked(true);
    ActionManager::addAction(Const::Action::ShowSidebar, showSidebarAction);

    QAction* showOutputAction = viewMenu->addAction(tr("Show Output"), outputTabWidget, &QTableWidget::setVisible);
    showOutputAction->setCheckable(true);
    showOutputAction->setChecked(true);
    ActionManager::addAction(Const::Action::ShowOutput, showOutputAction);

    QMenu* helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(tr("Documentation"), this, &MainWindow::onDocumentationAction);
    helpMenu->addAction(tr("Standard Library"), this, &MainWindow::onStandardLibraryAction);
    helpMenu->addAction(tr("The Book"), this, &MainWindow::onTheBookAction);
    helpMenu->addSeparator();
    helpMenu->addAction(tr("About..."), this, &MainWindow::onAboutAction);
}

void MainWindow::addRecentFile(const QString& filePath) {
    addRecentFileOrProject(recentFilesMenu, filePath, [=, this] {
        addSourceTab(filePath);
    });
}

void MainWindow::addRecentProject(const QString& projectPath) {
    if (!QFileInfo::exists(projectPath + "/Cargo.toml")) return;

    addRecentFileOrProject(recentProjectsMenu, projectPath, [=, this] {
        openProject(projectPath);
    });
}

void MainWindow::addRecentFileOrProject(QMenu* menu, const QString& filePath, const std::function<void()>& callback) {
    const auto actions = menu->actions();

    for (QAction* action : actions) {
        if (action->text() == filePath) {
            menu->removeAction(action);
        }
    }

    QAction* fileAction = new QAction(filePath);
    connect(fileAction, &QAction::triggered, callback);
    menu->insertAction(menu->actions().constFirst(), fileAction);

    if (menu->actions().size() > Const::Window::MaxRecentFiles + Const::Window::SeparatorAndMenuClearCount) {
        menu->removeAction(menu->actions().at(menu->actions().size() - Const::Window::SeparatorAndMenuClearCount - 1));
    }

    updateMenuState();
}

void MainWindow::saveProjectProperties() {
    if (projectPath.isEmpty()) return;

    QString path = projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::PropertyFile;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open project properties file for writing" << path;
        return;
    }

    QJsonObject obj;
    obj["target"] = static_cast<int>(projectProperties->buildTarget());
    obj["arguments"] = projectProperties->arguments();

    file.write(QJsonDocument(obj).toJson());
}

void MainWindow::loadProjectProperties() {
    if (projectPath.isEmpty()) return;

    QString path = projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::PropertyFile;
    if (!QFileInfo::exists(path)) {
        return;
    }

    projectProperties->setProject(projectPath);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open project properties file for reading" << path;
        return;
    }

    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    CargoManager::BuildTarget target = static_cast<CargoManager::BuildTarget>(obj["target"].toInt());
    projectProperties->setBuildTarget(target);
    projectProperties->setArguments(obj["arguments"].toString());
}

void MainWindow::loadSettings() {
    // Window geometry
    int width = Settings::value("window.geometry.width").toInt();
    int height = Settings::value("window.geometry.height").toInt();

    QJsonValue x = Settings::value("window.geometry.x");
    QJsonValue y = Settings::value("window.geometry.y");

    if (x.isNull() || y.isNull()) {
        // Center window in screen.
        resize(width, height);
        setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens().at(0)->availableGeometry()));
    } else {
        setGeometry(x.toInt(), y.toInt(), width, height);
    }

    // Window state
    Qt::WindowStates state = static_cast<Qt::WindowStates>(Settings::value("window.state").toInt());

    if (state == Qt::WindowMaximized || state == Qt::WindowFullScreen) {
        setWindowState(state);
    }

    // Splitter sizes
    QList<int> sizes;
    QJsonArray sizesArray = Settings::value("gui.mainWindow.splitters.main").toArray();

    for (const auto& size : sizesArray) {
        sizes.append(size.toInt());
    }

    mainSplitter->setSizes(sizes);

    sizesArray = Settings::value("gui.mainWindow.splitters.side").toArray();
    sizes.clear();

    for (const auto& size : qAsConst(sizesArray)) {
        sizes.append(size.toInt());
    }

    sideSplitter->setSizes(sizes);

    // Sidebar
    ActionManager::action(Const::Action::ShowSidebar)->setChecked(Settings::value("gui.mainWindow.sidebar.visible").toBool());
    sideTabWidget->setCurrentIndex(Settings::value("gui.mainWindow.sidebar.tab").toInt());

    // Output pane
    ActionManager::action(Const::Action::ShowOutput)->setChecked(Settings::value("gui.mainWindow.output.visible").toBool());
    outputTabWidget->setCurrentIndex(Settings::value("gui.mainWindow.output.tab").toInt());

    const QString& family = Settings::value("gui.output.cargo.font.family").toString();
    int size = Settings::value("gui.output.cargo.font.size").toInt();
    QFont font(family, size);
    cargoPlainTextEdit->document()->setDefaultFont(font);

    // Recent projects
    QJsonArray recentProjects = Settings::value("gui.mainWindow.recent.projects").toArray();

    for (int i = recentProjects.size() - 1; i >= 0; --i) {
        QString projectPath = recentProjects.at(i).toString();
        addRecentProject(projectPath);
    }

    // Recent files
    QJsonArray recentFiles = Settings::value("gui.mainWindow.recent.files").toArray();

    for (int i = recentFiles.size() - 1; i >= 0; --i) {
        QString filePath = recentFiles.at(i).toString();
        addRecentFile(filePath);
    }

    // Last project
    QString lastProject = Settings::value("gui.mainWindow.session.project").toString();

    if (QFileInfo::exists(lastProject + "/Cargo.toml")) {
        openProject(lastProject);
    }
}

void MainWindow::saveSettings() {
    // Window geometry
    if (windowState() == Qt::WindowNoState) {
        Settings::setValue("window.geometry.x", geometry().x());
        Settings::setValue("window.geometry.y", geometry().y());
        Settings::setValue("window.geometry.width", geometry().width());
        Settings::setValue("window.geometry.height", geometry().height());
    }

    // Window state
    Settings::setValue("window.state", static_cast<int>(windowState()));

    QAction* showSidebarAction = ActionManager::action(Const::Action::ShowSidebar);
    QAction* showOutputAction = ActionManager::action(Const::Action::ShowOutput);

    // Splitter sizes
    if (showSidebarAction->isChecked()) {
        QJsonArray sizesArray;
        const auto sizes = mainSplitter->sizes();

        for (int size : sizes) {
            sizesArray.append(QJsonValue(size));
        }
        Settings::setValue("gui.mainWindow.splitters.main", sizesArray);
    }

    if (showOutputAction->isChecked()) {
        QJsonArray sizesArray;
        const auto sizes = sideSplitter->sizes();

        for (int size : sizes) {
            sizesArray.append(QJsonValue(size));
        }
        Settings::setValue("gui.mainWindow.splitters.side", sizesArray);
    }

    // Sidebar
    Settings::setValue("gui.mainWindow.sidebar.visible", showSidebarAction->isChecked());
    Settings::setValue("gui.mainWindow.sidebar.tab", sideTabWidget->currentIndex());

    // Output pane
    Settings::setValue("gui.mainWindow.output.visible", showOutputAction->isChecked());
    Settings::setValue("gui.mainWindow.output.tab", outputTabWidget->currentIndex());

    // Recent projects
    QJsonArray recentProjects;

    for (int i = 0; i < recentProjectsMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount; ++i) {
        recentProjects.append(recentProjectsMenu->actions().at(i)->text());
    }

    Settings::setValue("gui.mainWindow.recent.projects", recentProjects);

    // Recent files
    QJsonArray recentFiles;

    for (int i = 0; i < recentFilesMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount; ++i) {
        recentFiles.append(recentFilesMenu->actions().at(i)->text());
    }

    Settings::setValue("gui.mainWindow.recent.files", recentFiles);

    // Last project
    Settings::setValue("gui.mainWindow.session.project", projectPath);

    Settings::flush();
}

void MainWindow::saveSession() {
    if (!Settings::value("gui.mainWindow.session.restore").toBool() || projectPath.isEmpty()) {
        return;
    }

    QDir dir(projectPath);
    bool result = dir.mkdir(Const::Project::DataDir);
#ifdef Q_OS_WIN
    // Set hidden attribute on created directory (need only for Windows).
    if (result) {
        QString directory = projectPath + "/" + Const::Project::DataDir;
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

    QString path = projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::SessionFile;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open session file for writing" << path;
        return;
    }

    QJsonArray openFiles;
    for (int i = 0; i < sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(sourceTabWidget->widget(i));

        QJsonObject obj;
        obj["path"] = editor->getFilePath();

        QJsonArray cursorPosArray;
        QPoint pos = editor->getCursorPosition();
        cursorPosArray.append(pos.x());
        cursorPosArray.append(pos.y());
        obj["cursor"] = cursorPosArray;

        openFiles.append(obj);
    }

    QJsonObject obj;
    obj["openFiles"] = openFiles;
    obj["selectedTab"] = sourceTabWidget->currentIndex();

    file.write(QJsonDocument(obj).toJson());
}

void MainWindow::loadSession() {
    if (!Settings::value("gui.mainWindow.session.restore").toBool() || projectPath.isEmpty()) {
        return;
    }

    QString path = projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::SessionFile;
    if (!QFileInfo::exists(path)) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open session file for reading" << path;
        return;
    }

    QJsonObject session = QJsonDocument::fromJson(file.readAll()).object();
    QJsonArray array = session["openFiles"].toArray();
    int selectedTab = session["selectedTab"].toInt();
    QString selectedFilePath = array.at(selectedTab).toObject()["path"].toString();

    for (int i = 0; i < array.count(); i++) {
        QJsonObject obj = array.at(i).toObject();
        QString filePath = obj["path"].toString();
        if (QFileInfo::exists(filePath)) {
            int index = addSourceTab(filePath);
            QJsonArray cursorPosArray = obj["cursor"].toArray();
            TextEditor* editor = static_cast<TextEditor*>(sourceTabWidget->widget(index));
            editor->setCursorPosition(QPoint(cursorPosArray.at(0).toInt(), cursorPosArray.at(1).toInt()));
        }
    }

    int index = findSource(selectedFilePath);
    sourceTabWidget->setCurrentIndex(index);
    onSourceTabCurrentChanged(index);
}

void MainWindow::openProject(const QString& path, bool isNew) {
    closeProject();

    projectPath = path;
    projectTree->setRootPath(path);
    cargoManager->setProjectPath(path);
    RlsManager::initialize(path);

    if (isNew) {
        QString filePath = projectPath + "/src/main.rs";
        if (QFileInfo::exists(filePath)) {
            addSourceTab(filePath);
        } else {
            filePath = projectPath + "/src/lib.rs";
            if (QFileInfo::exists(filePath)) {
                addSourceTab(filePath);
            }
        }
        projectProperties->setProject(projectPath);
    } else {
        loadProjectProperties();
        loadSession();
    }

    if (!sourceTabWidget->count()) {
        changeWindowTitle();
    }

    updateMenuState();
    addRecentProject(path);
}

void MainWindow::closeProject() {
    if (projectPath.isNull()) return;

    RlsManager::shutdown();

    saveSession();
    saveProjectProperties();

    onCloseAllAction();

    projectProperties->reset();
    projectTree->setRootPath(QString());
    projectPath = QString();
    changeWindowTitle();
    updateMenuState();
    cargoPlainTextEdit->clear();
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
    for (int i = 0; i < sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(sourceTabWidget->widget(i));
        if (editor->getFilePath() == filePath) {
            return i;
        }
    }

    return -1;
}

void MainWindow::updateMenuState() {
    editMenu->menuAction()->setVisible(!projectPath.isNull());
    buildMenu->menuAction()->setVisible(!projectPath.isNull());

    int index = sourceTabWidget->currentIndex();

    ActionManager::action(Const::Action::Save)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::SaveAs)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::SaveAll)->setEnabled(index >= 0);

    ActionManager::action(Const::Action::Close)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::CloseAll)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::CloseOther)->setEnabled(index >= 0);

    recentProjectsMenu->menuAction()->setEnabled(recentProjectsMenu->actions().size() > Const::Window::SeparatorAndMenuClearCount);
    recentFilesMenu->menuAction()->setEnabled(recentFilesMenu->actions().size() > Const::Window::SeparatorAndMenuClearCount);

    editMenu->menuAction()->setEnabled(index >= 0);
}

void MainWindow::prepareBuild() {
    onSaveAllAction();
    int index = static_cast<int>(OutputPane::Cargo);
    outputTabWidget->setCurrentIndex(index);
    issueModel->clear();
}
