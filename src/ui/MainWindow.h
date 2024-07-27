#pragma once
#include <QMainWindow>
#include <functional>

class QSplitter;
class QPlainTextEdit;

class SyntaxHighlightManager;
class CargoManager;
class ProjectTree;
class ProjectProperties;
class TextEditor;
class AutoCompleter;
class IssueModel;
class IssueListView;
class SourceTabWidget;

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
    void onDocAction();

    // Tools
    void onRustInstallerAction();
    void onPreferencesAction();

    // Help
    void onDocumentationAction();
    void onStandardLibraryAction();
    void onTheBookAction();
    void onAboutAction();

    // TabWidet
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
    void updateMenuState();
    void prepareBuild();

    QMenu* m_recentFilesMenu = nullptr;
    QMenu* m_recentProjectsMenu = nullptr;

    QMenu* m_editMenu = nullptr;
    QMenu* m_buildMenu = nullptr;

    QSplitter* m_mainSplitter = nullptr;
    QSplitter* m_sideSplitter = nullptr;

    QTabWidget* m_sideTabWidget = nullptr;
    SourceTabWidget* m_sourceTabWidget = nullptr;
    QTabWidget* m_outputTabWidget = nullptr;

    QPlainTextEdit* m_cargoPlainTextEdit = nullptr;

    SyntaxHighlightManager* m_syntaxHighlightManager = nullptr;
    CargoManager* m_cargoManager = nullptr;
    ProjectTree* m_projectTree = nullptr;
    ProjectProperties* m_projectProperties = nullptr;
    QString m_projectPath;
    TextEditor* m_editor = nullptr;
    AutoCompleter* m_completer = nullptr;
    IssueModel* m_issueModel = nullptr;
    IssueListView* m_issueListView = nullptr;
};
