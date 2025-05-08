#include "MainWindow.h"
#include "NewProject.h"
#include "GoToLine.h"
#include "Preferences.h"
#include "ProjectTree.h"
#include "ProjectProperties.h"
#include "IssueListView.h"
#include "IssueModel.h"
#include "ActionManager.h"
#include "SourceTabWidget.h"
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
#include "widgets/IconToolButton.h"
#include "widgets/RecentMenu.h"
#include "process/RlsManager.h"
#include <QSplitter>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QScrollBar>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonParseError>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

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

    m_sourceTabWidget = new SourceTabWidget(m_sideSplitter);
    m_sideSplitter->addWidget(m_sourceTabWidget);

    connect(m_sourceTabWidget, &QTabWidget::tabBarClicked, this, &MainWindow::onSourceTabCurrentChanged);

    m_outputTabWidget = new QTabWidget(m_sideSplitter);
    m_outputTabWidget->setMinimumSize(QSize(0, 50));

    auto cargoTab = new QWidget();

    auto cargoHorizontalLayout = new QHBoxLayout(cargoTab);
    cargoHorizontalLayout->setSpacing(0);
    cargoHorizontalLayout->setContentsMargins(QMargins());

    auto cargoVerticalLayout = new QVBoxLayout();
    cargoVerticalLayout->setSpacing(0);
    cargoVerticalLayout->setContentsMargins(1, 1, 1, 1);

    auto buildToolButtonCargo = new IconToolButton;
    buildToolButtonCargo->setToolTip(tr("Build"));
    buildToolButtonCargo->setText(Const::FontAwesome::Hammer);
    cargoVerticalLayout->addWidget(buildToolButtonCargo);

    auto runToolButtonCargo = new IconToolButton;
    runToolButtonCargo->setToolTip(tr("Run"));
    runToolButtonCargo->setText(Const::FontAwesome::Play);
    cargoVerticalLayout->addWidget(runToolButtonCargo);

    auto stopToolButtonCargo = new IconToolButton;
    stopToolButtonCargo->setToolTip(tr("Stop"));
    stopToolButtonCargo->setText(Const::FontAwesome::Stop);
    cargoVerticalLayout->addWidget(stopToolButtonCargo);

    auto clearToolButtonCargo = new IconToolButton;
    clearToolButtonCargo->setToolTip(tr("Clear"));
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

    auto issuesTab = new QWidget();
    auto issueHorizontalLayout = new QHBoxLayout(issuesTab);
    issueHorizontalLayout->setSpacing(0);
    issueHorizontalLayout->setContentsMargins(QMargins());

    auto issueVerticalLayout = new QVBoxLayout();
    issueVerticalLayout->setSpacing(0);

    auto clearToolButtonIssues = new IconToolButton;
    clearToolButtonIssues->setToolTip(tr("Clear"));
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
    connect(m_projectTree, &ProjectTree::removeActivated, m_sourceTabWidget, &SourceTabWidget::closeFile);
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

    m_cargoManager->createProject(newCargoProject.target(), newCargoProject.path());
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

void MainWindow::onDocAction() {
    m_cargoManager->doc();
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
    Utils::runRustupCommand({ "doc", "--std" });
}

void MainWindow::onTheBookAction() {
    Utils::runRustupCommand({ "doc", "--book" });
}

void MainWindow::onAboutAction() {
    QMessageBox::about(this, tr("About %1").arg(Application::Name),
        tr("<h3>%1 %2 %3</h3> \
            IDE for Rust programming language<br><br> \
            Based on Qt %4<br> \
            Build on %5 %6<br><br> \
            <a href=%7>%7</a><br><br> \
            Copyright © %8, Vladimir Zarypov"
        ).arg(Application::Name, Application::Version, Application::Status,
            QT_VERSION_STR, Application::BuildDate, Application::BuildTime, Application::Url, Application::Years));
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

void MainWindow::onFileRenamed(const QString& oldPath, const QString& newPath) {
    for (int i = 0; i < m_sourceTabWidget->count(); ++i) {
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
    int tabIndex = m_sourceTabWidget->findTab(filePath);

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

        m_recentFilesMenu->addRecent(filePath);

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
    auto fileMenu = menuBar()->addMenu(tr("File"));
    auto newFileMenu = fileMenu->addMenu(tr("New"));

    ActionManager::addAction(Const::Action::NewProject, newFileMenu->addAction(tr("Project..."), this, &MainWindow::onNewProjectAction));
    ActionManager::addAction(Const::Action::NewRustFile, newFileMenu->addAction(tr("Rust File..."), Qt::CTRL | Qt::Key_N, this, &MainWindow::onNewRustFileAction));
    ActionManager::addAction(Const::Action::NewFile, newFileMenu->addAction(tr("File..."), this, &MainWindow::onNewFileAction));
    ActionManager::addAction(Const::Action::NewDirectory, newFileMenu->addAction(tr("Directory..."), this, &MainWindow::onNewDirectoryAction));

    ActionManager::addAction(Const::Action::Open, fileMenu->addAction(tr("Open..."), Qt::CTRL | Qt::Key_O, this, &MainWindow::onOpenAction));
    ActionManager::addAction(Const::Action::CloseProject, fileMenu->addAction(tr("Close Project"), this, &MainWindow::onCloseProjectAction));

    m_recentFilesMenu = new RecentMenu(tr("Recent Files"), fileMenu);
    fileMenu->addMenu(m_recentFilesMenu);
    connect(m_recentFilesMenu, &RecentMenu::activated, this, &MainWindow::addSourceTab);

    m_recentProjectsMenu = new RecentMenu("Recent Projects", fileMenu);
    fileMenu->addMenu(m_recentProjectsMenu);
    connect(m_recentProjectsMenu, &RecentMenu::activated, this, [this] (const QString& recent) { openProject(recent); });

    fileMenu->addSeparator();

    ActionManager::addAction(Const::Action::Save, fileMenu->addAction(tr("Save"), Qt::CTRL | Qt::Key_S, this, &MainWindow::onSaveAction));
    ActionManager::addAction(Const::Action::SaveAs, fileMenu->addAction(tr("Save As..."), this, &MainWindow::onSaveAsAction));
    ActionManager::addAction(Const::Action::SaveAll, fileMenu->addAction(tr("Save All"), Qt::CTRL | Qt::SHIFT | Qt::Key_S, m_sourceTabWidget, &SourceTabWidget::saveAll));

    fileMenu->addSeparator();

    ActionManager::addAction(Const::Action::Close, fileMenu->addAction(tr("Close"), Qt::CTRL | Qt::Key_W, m_sourceTabWidget, &SourceTabWidget::closeCurrentTab));
    ActionManager::addAction(Const::Action::CloseAll, fileMenu->addAction(tr("Close All"), Qt::CTRL | Qt::SHIFT | Qt::Key_W, m_sourceTabWidget, &SourceTabWidget::closeAll));
    ActionManager::addAction(Const::Action::CloseOther, fileMenu->addAction(tr("Close Other"), m_sourceTabWidget, &SourceTabWidget::closeOthers));

    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"), Qt::CTRL | Qt::Key_Q, this, &MainWindow::close);

    m_editMenu = menuBar()->addMenu(tr("Edit"));
    m_editMenu->addAction(tr("Undo"), Qt::CTRL | Qt::Key_Z, m_editor, &QPlainTextEdit::undo);
    m_editMenu->addAction(tr("Redo"), Qt::CTRL | Qt::SHIFT | Qt::Key_N, m_editor, &QPlainTextEdit::redo);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Cut"), Qt::CTRL | Qt::Key_X, m_editor, &QPlainTextEdit::cut);
    m_editMenu->addAction(tr("Copy"), Qt::CTRL | Qt::Key_C, m_editor, &QPlainTextEdit::copy);
    m_editMenu->addAction(tr("Paste"), Qt::CTRL | Qt::Key_V, m_editor, &QPlainTextEdit::paste);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Select All"), Qt::CTRL | Qt::Key_A, m_editor, &QPlainTextEdit::selectAll);
    m_editMenu->addSeparator();

    auto lineMenu = m_editMenu->addMenu(tr("Line"));
    lineMenu->addAction(tr("Duplicate Line"), Qt::CTRL | Qt::Key_D, this, &MainWindow::onDuplicateLineAction);
    lineMenu->addAction(tr("Join Lines"), Qt::CTRL | Qt::Key_J, this, &MainWindow::onJoinLinesAction);
    lineMenu->addAction(tr("Cut Line"), Qt::CTRL | Qt::Key_Delete, this, &MainWindow::onCutLineAction);

    auto indentMenu = m_editMenu->addMenu(tr("Indent"));
    indentMenu->addAction(tr("Increase Indent"), Qt::Key_Tab, this, &MainWindow::onIncreaseIndentAction);
    indentMenu->addAction(tr("Decrease Indent"), Qt::SHIFT | Qt::Key_Tab, this, &MainWindow::onDecreaseIndentAction);

    auto commentMenu = m_editMenu->addMenu(tr("Comment"));
    commentMenu->addAction(tr("Toggle Single Line Comment"), Qt::CTRL | Qt::Key_Slash, this, &MainWindow::onToggleSingleLineCommentAction);
    commentMenu->addAction(tr("Toggle Block Comment"), Qt::CTRL | Qt::SHIFT | Qt::Key_Slash, this, &MainWindow::onToggleBlockCommentAction);

    m_editMenu->addAction(tr("Auto-Completer"), Qt::CTRL | Qt::Key_Space, this, &MainWindow::onAutoCompleterAction);
    m_editMenu->addAction(tr("Clean Trailing Whitespace"), this, &MainWindow::onCleanTrailingWhitespaceAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Go to Line..."), Qt::CTRL | Qt::Key_G, this, &MainWindow::onGoToLineAction);

    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("Preferences..."), this, &MainWindow::onPreferencesAction);

    m_buildMenu = menuBar()->addMenu(tr("Build"));
    ActionManager::addAction(Const::Action::Build, m_buildMenu->addAction(tr("Build"), Qt::CTRL | Qt::Key_B, this, &MainWindow::onBuildAction));
    ActionManager::addAction(Const::Action::Run, m_buildMenu->addAction(tr("Run"), Qt::CTRL | Qt::Key_R, this, &MainWindow::onRunAction));
    ActionManager::addAction(Const::Action::Check, m_buildMenu->addAction(tr("Check"), Qt::CTRL | Qt::Key_K, this, &MainWindow::onCheckAction));
    ActionManager::addAction(Const::Action::Stop, m_buildMenu->addAction(tr("Stop"), this, &MainWindow::onStopAction));
    ActionManager::addAction(Const::Action::Clean, m_buildMenu->addAction(tr("Clean"), this, &MainWindow::onCleanAction));
    ActionManager::addAction(Const::Action::Doc, m_buildMenu->addAction(tr("Doc"), this, &MainWindow::onDocAction));

    auto toolsMenu = menuBar()->addMenu(tr("Tools"));
    toolsMenu->addAction(tr("Rust Installer..."), this, &MainWindow::onRustInstallerAction);

    auto viewMenu = menuBar()->addMenu(tr("View"));

    auto showSidebarAction = viewMenu->addAction(tr("Show Sidebar"), m_sideTabWidget, &QTableWidget::setVisible);
    showSidebarAction->setCheckable(true);
    showSidebarAction->setChecked(true);
    ActionManager::addAction(Const::Action::ShowSidebar, showSidebarAction);

    auto showOutputAction = viewMenu->addAction(tr("Show Output"), m_outputTabWidget, &QTableWidget::setVisible);
    showOutputAction->setCheckable(true);
    showOutputAction->setChecked(true);
    ActionManager::addAction(Const::Action::ShowOutput, showOutputAction);

    auto helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(tr("Documentation"), this, &MainWindow::onDocumentationAction);
    helpMenu->addAction(tr("Standard Library"), this, &MainWindow::onStandardLibraryAction);
    helpMenu->addAction(tr("The Book"), this, &MainWindow::onTheBookAction);
    helpMenu->addSeparator();
    helpMenu->addAction(tr("About..."), this, &MainWindow::onAboutAction);
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

    QVariant x = Settings::value("window.geometry.x");
    QVariant y = Settings::value("window.geometry.y");

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
    QVariantList sizesArray = Settings::value("gui.mainWindow.splitters.main").toList();

    for (const auto& size : sizesArray) {
        sizes.append(size.toInt());
    }

    m_mainSplitter->setSizes(sizes);

    sizesArray = Settings::value("gui.mainWindow.splitters.side").toList();
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
    QVariantList recentProjects = Settings::value("gui.mainWindow.recent.projects").toList();

    for (int i = recentProjects.size() - 1; i >= 0; --i) {
        m_recentProjectsMenu->addRecent(recentProjects.at(i).toString());
    }

    // Recent files
    QVariantList recentFiles = Settings::value("gui.mainWindow.recent.files").toList();

    for (int i = recentFiles.size() - 1; i >= 0; --i) {
        m_recentFilesMenu->addRecent(recentFiles.at(i).toString());
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

    // Recents
    Settings::setValue("gui.mainWindow.recent.projects", m_recentProjectsMenu->recents());
    Settings::setValue("gui.mainWindow.recent.files", m_recentFilesMenu->recents());

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

    for (int i = 0; i < m_sourceTabWidget->count(); ++i) {
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

    for (int i = 0; i < array.count(); ++i) {
        QJsonObject obj = array.at(i).toObject();
        QString filePath = obj["path"].toString();

        if (QFileInfo::exists(filePath)) {
            int index = addSourceTab(filePath);
            QJsonArray cursorPosArray = obj["cursor"].toArray();
            TextEditor* editor = static_cast<TextEditor*>(m_sourceTabWidget->widget(index));
            editor->setCursorPosition(QPoint(cursorPosArray.at(0).toInt(), cursorPosArray.at(1).toInt()));
        }
    }

    int index = m_sourceTabWidget->findTab(selectedFilePath);
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
    m_recentProjectsMenu->addRecent(path);
}

void MainWindow::closeProject() {
    if (m_projectPath.isNull()) return;

    RlsManager::shutdown();

    saveSession();
    saveProjectProperties();

    m_sourceTabWidget->closeAll();

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

    m_recentProjectsMenu->updateState();
    m_recentFilesMenu->updateState();

    m_editMenu->menuAction()->setEnabled(index >= 0);
}

void MainWindow::prepareBuild() {
    m_sourceTabWidget->saveAll();
    int index = static_cast<int>(OutputPane::Cargo);
    m_outputTabWidget->setCurrentIndex(index);
    m_issueModel->clear();
}
