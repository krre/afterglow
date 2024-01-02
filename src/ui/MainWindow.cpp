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
#include "core/Application.h"
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
    m_syntaxHighlightManager = new SyntaxHighlightManager(this);
    new ActionManager(this);

    resize(1040, 620);

    m_mainSplitter = new QSplitter;
    m_mainSplitter->setOrientation(Qt::Horizontal);
    m_mainSplitter->setHandleWidth(0);
    m_mainSplitter->setChildrenCollapsible(false);

    setCentralWidget(m_mainSplitter);

    m_sideTabWidget = new QTabWidget(m_mainSplitter);
    m_sideTabWidget->setMinimumSize(QSize(50, 0));
    m_sideTabWidget->setBaseSize(QSize(200, 0));
    m_sideTabWidget->setTabPosition(QTabWidget::West);
    m_mainSplitter->addWidget(m_sideTabWidget);

    m_sideSplitter = new QSplitter(m_mainSplitter);
    m_sideSplitter->setOrientation(Qt::Vertical);
    m_sideSplitter->setHandleWidth(0);
    m_sideSplitter->setChildrenCollapsible(false);

    m_sourceTabWidget = new QTabWidget(m_sideSplitter);
    m_sourceTabWidget->setMinimumSize(QSize(0, 50));
    m_sourceTabWidget->setTabsClosable(true);
    m_sourceTabWidget->setMovable(true);
    m_sideSplitter->addWidget(m_sourceTabWidget);

    connect(m_sourceTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onSourceTabClose);
    connect(m_sourceTabWidget, &QTabWidget::tabBarClicked, this, &MainWindow::onSourceTabCurrentChanged);

    m_outputTabWidget = new QTabWidget(m_sideSplitter);
    m_outputTabWidget->setMinimumSize(QSize(0, 50));

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

    m_cargoPlainTextEdit = new QPlainTextEdit(cargoTab);
    m_cargoPlainTextEdit->setFocusPolicy(Qt::StrongFocus);
    m_cargoPlainTextEdit->setReadOnly(true);

    cargoHorizontalLayout->addWidget(m_cargoPlainTextEdit);

    m_outputTabWidget->addTab(cargoTab, tr("Cargo"));

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

    m_outputTabWidget->addTab(issuesTab, tr("Issues"));

    m_sideSplitter->addWidget(m_outputTabWidget);
    m_mainSplitter->addWidget(m_sideSplitter);

    m_sideTabWidget->setCurrentIndex(-1);
    m_outputTabWidget->setCurrentIndex(0);

    m_projectProperties = new ProjectProperties;

    m_cargoManager = new CargoManager(m_projectProperties, this);
    connect(m_cargoManager, &CargoManager::projectCreated, this, &MainWindow::onProjectCreated);
    connect(m_cargoManager, &CargoManager::consoleMessage, this, &MainWindow::onCargoMessage);

    m_projectTree = new ProjectTree;
    connect(m_projectTree, &ProjectTree::openActivated, this, &MainWindow::addSourceTab);
    connect(m_projectTree, &ProjectTree::newFileActivated, this, &MainWindow::onFileCreated);
    connect(m_projectTree, &ProjectTree::removeActivated, this, &MainWindow::onFileRemoved);
    connect(m_projectTree, &ProjectTree::renameActivated, this, &MainWindow::onFileRenamed);

    m_sideTabWidget->addTab(m_projectTree, tr("Project"));
    m_sideTabWidget->addTab(m_projectProperties, tr("Properties"));

    m_issueModel = new IssueModel(this);
    connect(m_issueModel, &IssueModel::countChanged, this, &MainWindow::onIssueCountChanged);
    m_issueListView = new IssueListView(m_issueModel);
    connect(m_issueListView, &IssueListView::doubleClicked, this, [this] (const QModelIndex& index) {
        QString filename = m_issueModel->data(index, static_cast<int>(IssueModel::Role::Filename)).toString();
        if (!filename.isEmpty()) {
            QString line = m_issueModel->data(index, static_cast<int>(IssueModel::Role::Line)).toString();
            addSourceTab(m_projectPath + "/" + filename);
            m_editor->goToLine(line.toInt());
        }
    });

    issueHorizontalLayout->addWidget(m_issueListView);

    new RlsManager(this);
    RlsManager::setShowDebug(Settings::value("debug.dump.rlsMessages").toBool());
//    RlsManager::start();

    m_completer = new AutoCompleter(this);
    m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setWrapAround(false);

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
    m_cargoManager->createProject(newCargoProject.projectTemplate(), newCargoProject.path());
}

void MainWindow::onNewRustFileAction() {
    m_projectTree->onNewRustFile();
}

void MainWindow::onCloseProjectAction() {
    closeProject();
}

void MainWindow::onSaveAsAction() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), m_editor->filePath(), "Rust (*.rs);;All Files(*.*)");
    if (!filePath.isEmpty()) {
        bool result = QFile::copy(m_editor->filePath(), filePath);
        if (result) {
            addSourceTab(filePath);
        } else {
            QMessageBox::critical(this, tr("Duplicate"), tr("Error creating copy of file"));
        }
    }
}

void MainWindow::onSaveAllAction() {
    for (int i = 0; i < m_sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(i));
        editor->saveFile();
    }
}

void MainWindow::onClearMenuRecentFilesAction() {
    for (int i = m_recentFilesMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount - 1; i >= 0; i--) {
        m_recentFilesMenu->removeAction(m_recentFilesMenu->actions().at(i));
    }

    updateMenuState();
}

void MainWindow::onClearMenuRecentProjectsAction() {
    for (int i = m_recentProjectsMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount - 1; i >= 0; i--) {
        m_recentProjectsMenu->removeAction(m_recentProjectsMenu->actions().at(i));
    }

    updateMenuState();
}

void MainWindow::onCloseAction() {
    onSourceTabClose(m_sourceTabWidget->currentIndex());
}

void MainWindow::onCloseAllAction() {
    while (m_sourceTabWidget->count()) {
        onSourceTabClose(0);
    }
}

void MainWindow::onCloseOtherAction() {
    int i = 0;

    while (m_sourceTabWidget->count() > 1) {
        if (i != m_sourceTabWidget->currentIndex()) {
            onSourceTabClose(i);
        } else {
            i++;
        }
    }
}

void MainWindow::onNewFileAction() {
    m_projectTree->onNewFile();
}

void MainWindow::onNewDirectoryAction() {
    m_projectTree->onNewDirectory();
}

void MainWindow::onOpenAction() {
    QString dirPath = m_projectPath.isEmpty() ? Global::workspacePath() : m_projectPath;
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
    m_editor->saveFile();
}

void MainWindow::onUndoAction() {
    m_editor->undo();
}

void MainWindow::onRedoAction() {
    m_editor->redo();
}

void MainWindow::onCutAction() {
    m_editor->cut();
}

void MainWindow::onCopyAction() {
    m_editor->copy();
}

void MainWindow::onPasteAction() {
    m_editor->paste();
}

void MainWindow::onSelectAllAction() {
    m_editor->selectAll();
}

void MainWindow::onToggleSingleLineCommentAction() {
    m_editor->toggleSingleLineComment();
}

void MainWindow::onToggleBlockCommentAction() {
    m_editor->toggleBlockComment();
}

void MainWindow::onAutoCompleterAction() {
    m_editor->openAutoCompleter();
}

void MainWindow::onJoinLinesAction() {
    m_editor->joinLines();
}

void MainWindow::onDuplicateLineAction() {
    m_editor->duplicateLine();
}

void MainWindow::onCutLineAction() {
    m_editor->cutLine();
}

void MainWindow::onIncreaseIndentAction() {
    m_editor->increaseIndent();
}

void MainWindow::onDecreaseIndentAction() {
    m_editor->decreaseIndent();
}

void MainWindow::onGoToLineAction() {
    GoToLine goToLine(this);
    if (goToLine.exec() == QDialog::Rejected) return;
    m_editor->goToLine(goToLine.line());
}

void MainWindow::onCleanTrailingWhitespaceAction() {
    m_editor->cleanTrailingWhitespace();
}

void MainWindow::onBuildAction() {
    prepareBuild();
    m_cargoManager->build();
}

void MainWindow::onRunAction() {
    prepareBuild();
    m_cargoManager->run();
}

void MainWindow::onCheckAction() {
    prepareBuild();
    m_cargoManager->check();
}

void MainWindow::onStopAction() {
    m_cargoManager->stop();
}

void MainWindow::onCleanAction() {
    m_cargoManager->clean();
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
    QMessageBox::about(this, tr("About %1").arg(Application::Name),
        tr("<h3>%1 %2 %3</h3>\
           IDE for Rust programming language<br><br> \
           Based on Qt %4<br> \
           Build on %5 %6<br><br> \
           <a href=%7>%7</a><br><br>%8")
            .arg(Application::Name, Application::Version, Application::Status,
            QT_VERSION_STR, Application::BuildDate, Application::BuildTime, Application::Url, Application::Copyright));
}

void MainWindow::onSourceTabClose(int index) {
    QWidget* widget = m_sourceTabWidget->widget(index);
    m_sourceTabWidget->removeTab(index);
    delete widget;
}

void MainWindow::onSourceTabCurrentChanged(int index) {
    if (index >= 0) {
        m_editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(index));
        m_editor->setAutoCompleter(m_completer);
        m_editor->setFocus();
        QString filePath = m_editor->filePath();
        m_projectTree->selectFile(filePath);
        changeWindowTitle(filePath);
    } else {
        m_editor = nullptr;
        m_projectTree->setCurrentIndex(QModelIndex());
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
    m_cargoPlainTextEdit->clear();
}

void MainWindow::onCargoStop() {
    m_cargoManager->stop();
}

void MainWindow::onIssuesClear() {
    m_issueListView->setCurrentIndex(QModelIndex());
    m_issueModel->clear();
}

void MainWindow::onProjectCreated(const QString& path) {
    openProject(path, true);
}

void MainWindow::onCargoMessage(const QString& message, bool html, bool start) {
    if (start) {
        m_cargoPlainTextEdit->clear();
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
                        m_issueModel->appendMessage(obj);
                    }
                }
            }
        }
    }

    if (isJson) return;

    if (html) {
        m_cargoPlainTextEdit->appendHtml(message);
    } else {
        m_cargoPlainTextEdit->insertPlainText(message);
    }

    m_cargoPlainTextEdit->verticalScrollBar()->setValue(m_cargoPlainTextEdit->verticalScrollBar()->maximum());
}

void MainWindow::onFileCreated(const QString& filePath) {
    addNewFile(filePath);
}

void MainWindow::onFileRemoved(const QString& filePath) {
    QVector<int> indices;
    for (int i = 0; i < m_sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(i));
        if (editor->filePath().contains(filePath)) {
            indices.append(i);
        }
    }

    // Remove tabs in reverse order
    for (int i = indices.count() - 1; i >= 0; i--) {
        onSourceTabClose(indices.at(i));
    }
}

void MainWindow::onFileRenamed(const QString& oldPath, const QString& newPath) {
    for (int i = 0; i < m_sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(i));
        if (editor->filePath().contains(oldPath)) {
            QFileInfo fi(newPath);
            if (fi.isDir()) {
                editor->setFilePath(editor->filePath().replace(oldPath, newPath));
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
        m_sourceTabWidget->setCurrentIndex(tabIndex);
        return tabIndex;
    } else {
        QFileInfo fi(filePath);
        m_editor = new TextEditor(filePath, m_syntaxHighlightManager);
        connect(m_editor, &TextEditor::documentModified, this, &MainWindow::onDocumentModified);
        int index = m_sourceTabWidget->addTab(m_editor, fi.fileName());
        m_sourceTabWidget->setTabToolTip(index, filePath);
        m_sourceTabWidget->setCurrentIndex(index);

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
    m_sourceTabWidget->setTabText(m_sourceTabWidget->indexOf(editor), editor->modifiedName());
}

void MainWindow::onIssueCountChanged(int count) {
    QString title = tr("Issues");
    if (count) {
        title += QString(" (%1)").arg(count);
    }

    m_outputTabWidget->setTabText(static_cast<int>(OutputPane::Issues), title);
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

    m_recentFilesMenu = fileMenu->addMenu(tr("Recent Files"));
    m_recentFilesMenu->addSeparator();
    m_recentFilesMenu->addAction(tr("Clear Menu"), this, &MainWindow::onClearMenuRecentFilesAction);

    m_recentProjectsMenu = fileMenu->addMenu(tr("Recent Projects"));
    m_recentProjectsMenu->addSeparator();
    m_recentProjectsMenu->addAction(tr("Clear Menu"), this, &MainWindow::onClearMenuRecentProjectsAction);

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

    m_editMenu = menuBar()->addMenu(tr("Edit"));
    m_editMenu->addAction(tr("Undo"), Qt::CTRL | Qt::Key_Z, this, &MainWindow::onUndoAction);
    m_editMenu->addAction(tr("Redo"), Qt::CTRL | Qt::SHIFT | Qt::Key_N, this, &MainWindow::onRedoAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Cut"), Qt::CTRL | Qt::Key_X, this, &MainWindow::onCutAction);
    m_editMenu->addAction(tr("Copy"), Qt::CTRL | Qt::Key_C, this, &MainWindow::onCopyAction);
    m_editMenu->addAction(tr("Paste"), Qt::CTRL | Qt::Key_V, this, &MainWindow::onPasteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Select All"), Qt::CTRL | Qt::Key_A, this, &MainWindow::onSelectAllAction);
    m_editMenu->addSeparator();

    QMenu* lineMenu = m_editMenu->addMenu(tr("Line"));
    lineMenu->addAction(tr("Duplicate Line"), Qt::CTRL | Qt::Key_D, this, &MainWindow::onDuplicateLineAction);
    lineMenu->addAction(tr("Join Lines"), Qt::CTRL | Qt::Key_J, this, &MainWindow::onJoinLinesAction);
    lineMenu->addAction(tr("Cut Line"), Qt::CTRL | Qt::Key_Delete, this, &MainWindow::onCutLineAction);

    QMenu* indentMenu = m_editMenu->addMenu(tr("Indent"));
    indentMenu->addAction(tr("Increase Indent"), Qt::Key_Tab, this, &MainWindow::onIncreaseIndentAction);
    indentMenu->addAction(tr("Decrease Indent"), Qt::SHIFT | Qt::Key_Tab, this, &MainWindow::onDecreaseIndentAction);

    QMenu* commentMenu = m_editMenu->addMenu(tr("Comment"));
    commentMenu->addAction(tr("Toggle Single Line Comment"), Qt::CTRL | Qt::Key_Slash, this, &MainWindow::onToggleSingleLineCommentAction);
    commentMenu->addAction(tr("Toggle Block Comment"), Qt::CTRL | Qt::SHIFT | Qt::Key_Slash, this, &MainWindow::onToggleBlockCommentAction);

    m_editMenu->addAction(tr("Auto-Completer"), Qt::CTRL | Qt::Key_Space, this, &MainWindow::onAutoCompleterAction);
    m_editMenu->addAction(tr("Clean Trailing Whitespace"), this, &MainWindow::onCleanTrailingWhitespaceAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Go to Line..."), Qt::CTRL | Qt::Key_G, this, &MainWindow::onGoToLineAction);

    m_buildMenu = menuBar()->addMenu(tr("Build"));
    ActionManager::addAction(Const::Action::Build, m_buildMenu->addAction(tr("Build"), Qt::CTRL | Qt::Key_B, this, &MainWindow::onBuildAction));
    ActionManager::addAction(Const::Action::Run, m_buildMenu->addAction(tr("Run"), Qt::CTRL | Qt::Key_R, this, &MainWindow::onRunAction));
    ActionManager::addAction(Const::Action::Check, m_buildMenu->addAction(tr("Check"), Qt::CTRL | Qt::Key_K, this, &MainWindow::onCheckAction));
    ActionManager::addAction(Const::Action::Stop, m_buildMenu->addAction(tr("Stop"), this, &MainWindow::onStopAction));
    ActionManager::addAction(Const::Action::Clean, m_buildMenu->addAction(tr("Clean"), this, &MainWindow::onCleanAction));

    QMenu* toolsMenu = menuBar()->addMenu(tr("Tools"));
    toolsMenu->addAction(tr("Rust Installer..."), this, &MainWindow::onRustInstallerAction);

    QMenu* viewMenu = menuBar()->addMenu(tr("View"));

    QAction* showSidebarAction = viewMenu->addAction(tr("Show Sidebar"), m_sideTabWidget, &QTableWidget::setVisible);
    showSidebarAction->setCheckable(true);
    showSidebarAction->setChecked(true);
    ActionManager::addAction(Const::Action::ShowSidebar, showSidebarAction);

    QAction* showOutputAction = viewMenu->addAction(tr("Show Output"), m_outputTabWidget, &QTableWidget::setVisible);
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
    addRecentFileOrProject(m_recentFilesMenu, filePath, [=, this] {
        addSourceTab(filePath);
    });
}

void MainWindow::addRecentProject(const QString& projectPath) {
    if (!QFileInfo::exists(projectPath + "/Cargo.toml")) return;

    addRecentFileOrProject(m_recentProjectsMenu, projectPath, [=, this] {
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
    if (m_projectPath.isEmpty()) return;

    QString path = m_projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::PropertyFile;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open project properties file for writing" << path;
        return;
    }

    QJsonObject obj;
    obj["target"] = static_cast<int>(m_projectProperties->buildTarget());
    obj["arguments"] = m_projectProperties->arguments();

    file.write(QJsonDocument(obj).toJson());
}

void MainWindow::loadProjectProperties() {
    if (m_projectPath.isEmpty()) return;

    QString path = m_projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::PropertyFile;
    if (!QFileInfo::exists(path)) {
        return;
    }

    m_projectProperties->setProject(m_projectPath);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open project properties file for reading" << path;
        return;
    }

    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    CargoManager::BuildTarget target = static_cast<CargoManager::BuildTarget>(obj["target"].toInt());
    m_projectProperties->setBuildTarget(target);
    m_projectProperties->setArguments(obj["arguments"].toString());
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

    m_mainSplitter->setSizes(sizes);

    sizesArray = Settings::value("gui.mainWindow.splitters.side").toArray();
    sizes.clear();

    for (const auto& size : std::as_const(sizesArray)) {
        sizes.append(size.toInt());
    }

    m_sideSplitter->setSizes(sizes);

    // Sidebar
    ActionManager::action(Const::Action::ShowSidebar)->setChecked(Settings::value("gui.mainWindow.sidebar.visible").toBool());
    m_sideTabWidget->setCurrentIndex(Settings::value("gui.mainWindow.sidebar.tab").toInt());

    // Output pane
    ActionManager::action(Const::Action::ShowOutput)->setChecked(Settings::value("gui.mainWindow.output.visible").toBool());
    m_outputTabWidget->setCurrentIndex(Settings::value("gui.mainWindow.output.tab").toInt());

    const QString& family = Settings::value("gui.output.cargo.font.family").toString();
    int size = Settings::value("gui.output.cargo.font.size").toInt();
    QFont font(family, size);
    m_cargoPlainTextEdit->document()->setDefaultFont(font);

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
        const auto sizes = m_mainSplitter->sizes();

        for (int size : sizes) {
            sizesArray.append(QJsonValue(size));
        }
        Settings::setValue("gui.mainWindow.splitters.main", sizesArray);
    }

    if (showOutputAction->isChecked()) {
        QJsonArray sizesArray;
        const auto sizes = m_sideSplitter->sizes();

        for (int size : sizes) {
            sizesArray.append(QJsonValue(size));
        }
        Settings::setValue("gui.mainWindow.splitters.side", sizesArray);
    }

    // Sidebar
    Settings::setValue("gui.mainWindow.sidebar.visible", showSidebarAction->isChecked());
    Settings::setValue("gui.mainWindow.sidebar.tab", m_sideTabWidget->currentIndex());

    // Output pane
    Settings::setValue("gui.mainWindow.output.visible", showOutputAction->isChecked());
    Settings::setValue("gui.mainWindow.output.tab", m_outputTabWidget->currentIndex());

    // Recent projects
    QJsonArray recentProjects;

    for (int i = 0; i < m_recentProjectsMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount; ++i) {
        recentProjects.append(m_recentProjectsMenu->actions().at(i)->text());
    }

    Settings::setValue("gui.mainWindow.recent.projects", recentProjects);

    // Recent files
    QJsonArray recentFiles;

    for (int i = 0; i < m_recentFilesMenu->actions().size() - Const::Window::SeparatorAndMenuClearCount; ++i) {
        recentFiles.append(m_recentFilesMenu->actions().at(i)->text());
    }

    Settings::setValue("gui.mainWindow.recent.files", recentFiles);

    // Last project
    Settings::setValue("gui.mainWindow.session.project", m_projectPath);

    Settings::flush();
}

void MainWindow::saveSession() {
    if (!Settings::value("gui.mainWindow.session.restore").toBool() || m_projectPath.isEmpty()) {
        return;
    }

#ifdef Q_OS_WIN
    QDir dir(m_projectPath);
    bool result = dir.mkdir(Const::Project::DataDir);

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
#endif

    QString path = m_projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::SessionFile;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open session file for writing" << path;
        return;
    }

    QJsonArray openFiles;
    for (int i = 0; i < m_sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(i));

        QJsonObject obj;
        obj["path"] = editor->filePath();

        QJsonArray cursorPosArray;
        QPoint pos = editor->cursorPosition();
        cursorPosArray.append(pos.x());
        cursorPosArray.append(pos.y());
        obj["cursor"] = cursorPosArray;

        openFiles.append(obj);
    }

    QJsonObject obj;
    obj["openFiles"] = openFiles;
    obj["selectedTab"] = m_sourceTabWidget->currentIndex();

    file.write(QJsonDocument(obj).toJson());
}

void MainWindow::loadSession() {
    if (!Settings::value("gui.mainWindow.session.restore").toBool() || m_projectPath.isEmpty()) {
        return;
    }

    QString path = m_projectPath + "/" + Const::Project::DataDir + "/" + Const::Project::SessionFile;
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
            TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(index));
            editor->setCursorPosition(QPoint(cursorPosArray.at(0).toInt(), cursorPosArray.at(1).toInt()));
        }
    }

    int index = findSource(selectedFilePath);
    m_sourceTabWidget->setCurrentIndex(index);
    onSourceTabCurrentChanged(index);
}

void MainWindow::openProject(const QString& path, bool isNew) {
    closeProject();

    m_projectPath = path;
    m_projectTree->setRootPath(path);
    m_cargoManager->setProjectPath(path);
    RlsManager::initialize(path);

    if (isNew) {
        QString filePath = m_projectPath + "/src/main.rs";
        if (QFileInfo::exists(filePath)) {
            addSourceTab(filePath);
        } else {
            filePath = m_projectPath + "/src/lib.rs";
            if (QFileInfo::exists(filePath)) {
                addSourceTab(filePath);
            }
        }
        m_projectProperties->setProject(m_projectPath);
    } else {
        loadProjectProperties();
        loadSession();
    }

    if (!m_sourceTabWidget->count()) {
        changeWindowTitle();
    }

    updateMenuState();
    addRecentProject(path);
}

void MainWindow::closeProject() {
    if (m_projectPath.isNull()) return;

    RlsManager::shutdown();

    saveSession();
    saveProjectProperties();

    onCloseAllAction();

    m_projectProperties->reset();
    m_projectTree->setRootPath(QString());
    m_projectPath = QString();
    changeWindowTitle();
    updateMenuState();
    m_cargoPlainTextEdit->clear();
}

void MainWindow::changeWindowTitle(const QString& filePath) {
    QString title = QApplication::applicationName();

    if (!m_projectPath.isEmpty()) {
        QFileInfo fi(m_projectPath);
        title = fi.baseName() + " - " + title;
    }

    if (!filePath.isEmpty()) {
        QFileInfo fi(filePath);
        title = fi.fileName() + " - " + title;
    }

    setWindowTitle(title);
}

int MainWindow::findSource(const QString& filePath) {
    for (int i = 0; i < m_sourceTabWidget->count(); i++) {
        TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(i));
        if (editor->filePath() == filePath) {
            return i;
        }
    }

    return -1;
}

void MainWindow::updateMenuState() {
    m_editMenu->menuAction()->setVisible(!m_projectPath.isNull());
    m_buildMenu->menuAction()->setVisible(!m_projectPath.isNull());

    int index = m_sourceTabWidget->currentIndex();

    ActionManager::action(Const::Action::Save)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::SaveAs)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::SaveAll)->setEnabled(index >= 0);

    ActionManager::action(Const::Action::Close)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::CloseAll)->setEnabled(index >= 0);
    ActionManager::action(Const::Action::CloseOther)->setEnabled(index >= 0);

    m_recentProjectsMenu->menuAction()->setEnabled(m_recentProjectsMenu->actions().size() > Const::Window::SeparatorAndMenuClearCount);
    m_recentFilesMenu->menuAction()->setEnabled(m_recentFilesMenu->actions().size() > Const::Window::SeparatorAndMenuClearCount);

    m_editMenu->menuAction()->setEnabled(index >= 0);
}

void MainWindow::prepareBuild() {
    onSaveAllAction();
    int index = static_cast<int>(OutputPane::Cargo);
    m_outputTabWidget->setCurrentIndex(index);
    m_issueModel->clear();
}
