#pragma once
#include <QMainWindow>
#include <functional>

class QSplitter;
class QPlainTextEdit;

class CargoManager;
class ProjectTree;
class ProjectProperties;
class TextEditor;
class AutoCompleter;
class IssueModel;
class IssueListView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // File
    void onNewProjectAction();
    void onNewRustFileAction();
    void onNewFileAction();
    void onNewDirectoryAction();

    void onOpenAction();
    void onCloseProjectAction();
    void onClearMenuRecentFilesAction();
    void onClearMenuRecentProjectsAction();

    void onSaveAction();
    void onSaveAsAction();
    void onSaveAllAction();

    void onCloseAction();
    void onCloseAllAction();
    void onCloseOtherAction();

    // Edit
    void onUndoAction();
    void onRedoAction();

    void onCutAction();
    void onCopyAction();
    void onPasteAction();

    void onSelectAllAction();

    void onJoinLinesAction();
    void onDuplicateLineAction();
    void onCutLineAction();

    void onIncreaseIndentAction();
    void onDecreaseIndentAction();

    void onToggleSingleLineCommentAction();
    void onToggleBlockCommentAction();

    void onAutoCompleterAction();
    void onGoToLineAction();
    void onCleanTrailingWhitespaceAction();

    // Build
    void onBuildAction();
    void onRunAction();
    void onCheckAction();
    void onStopAction();
    void onCleanAction();

    // Tools
    void onRustInstallerAction();
    void onOptionsAction();

    // Help
    void onDocumentationAction();
    void onStandardLibraryAction();
    void onTheBookAction();
    void onAboutAction();

    // TabWidet
    void onSourceTabClose(int index);
    void onSourceTabCurrentChanged(int index);

    void onCargoBuild();
    void onCargoRun();
    void onCargoClear();
    void onCargoStop();

    void onIssuesClear();

    // CargoManager
    void onProjectCreated(const QString& path);
    void onCargoMessage(const QString& message, bool html, bool start);

    // ProjectTree
    void onFileCreated(const QString& filePath);
    void onFileRemoved(const QString& filePath);
    void onFileRenamed(const QString& oldPath, const QString& newPath);

    int addSourceTab(const QString& filePath);
    void addNewFile(const QString& filePath);

    // Editor
    void onDocumentModified(TextEditor* editor);

    // Output pane
    void onIssueCountChanged(int count);

private:
    enum class OutputPane {
        Cargo,
        Issues,
        Search
    };

    void createActions();

    void addRecentFile(const QString& filePath);
    void addRecentProject(const QString& projectPath);
    void addRecentFileOrProject(QMenu* menu, const QString& filePath, const std::function<void()>& callback);

    void saveProjectProperties();
    void loadProjectProperties();

    void loadSettings();
    void saveSettings();

    void saveSession();
    void loadSession();

    void openProject(const QString& path, bool isNew = false);
    void closeProject();

    void changeWindowTitle(const QString& filePath = QString());
    int findSource(const QString& filePath);
    void updateMenuState();
    void prepareBuild();

    QMenu* recentFilesMenu = nullptr;
    QMenu* recentProjectsMenu = nullptr;

    QMenu* editMenu = nullptr;
    QMenu* buildMenu = nullptr;

    QSplitter* mainSplitter = nullptr;
    QSplitter* sideSplitter = nullptr;

    QTabWidget* sideTabWidget = nullptr;
    QTabWidget* sourceTabWidget = nullptr;
    QTabWidget* outputTabWidget = nullptr;

    QPlainTextEdit* cargoPlainTextEdit = nullptr;

    CargoManager* cargoManager = nullptr;
    ProjectTree* projectTree = nullptr;
    ProjectProperties* projectProperties = nullptr;
    QString projectPath;
    TextEditor* editor = nullptr;
    AutoCompleter* completer = nullptr;
    IssueModel* issueModel = nullptr;
    IssueListView* issueListView = nullptr;
};
