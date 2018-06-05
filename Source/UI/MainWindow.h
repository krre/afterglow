#pragma once
#include <QMainWindow>
#include <functional>

class CargoManager;
class ApplicationManager;
class ProjectTree;
class ProjectProperties;
class TextEditor;
class AutoCompleter;
class IssueModel;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // File
    void on_actionNewProject_triggered();
    void on_actionNewRustFile_triggered();
    void on_actionNewFile_triggered();
    void on_actionNewDirectory_triggered();

    void on_actionOpen_triggered();
    void on_actionCloseProject_triggered();
    void on_actionClearMenuRecentFiles_triggered();
    void on_actionClearMenuRecentProjects_triggered();

    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionSaveAll_triggered();

    void on_actionClose_triggered();
    void on_actionCloseAll_triggered();
    void on_actionCloseOther_triggered();

    // Edit
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();

    void on_actionSelectAll_triggered();

    void on_actionJoinLines_triggered();
    void on_actionDuplicateLine_triggered();
    void on_actionCutLine_triggered();

    void on_actionIncreaseIndent_triggered();
    void on_actionDecreaseIndent_triggered();

    void on_actionToggleSingleLineComment_triggered();
    void on_actionToggleBlockComment_triggered();

    void on_actionAutoCompleter_triggered();
    void on_actionGoToLine_triggered();
    void on_actionCleanTrailingWhitespace_triggered();

    // Build
    void on_actionBuild_triggered();
    void on_actionRun_triggered();
    void on_actionStop_triggered();
    void on_actionClean_triggered();

    // Tools
    void on_actionRustInstaller_triggered();
    void on_actionOptions_triggered();

    // Help
    void on_actionDocumentation_triggered();
    void on_actionStandardLibrary_triggered();
    void on_actionTheBook_triggered();
    void on_actionAbout_triggered();

    // TabWidet
    void on_tabWidgetSource_tabCloseRequested(int index);
    void on_tabWidgetSource_currentChanged(int index);

    void on_toolButtonCargoBuild_clicked();
    void on_toolButtonCargoRun_clicked();
    void on_toolButtonCargoClear_clicked();
    void on_toolButtonCargoStop_clicked();

    void on_toolButtonIssuesClear_clicked();

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

private:
    void addRecentFile(const QString& filePath);
    void addRecentProject(const QString& projectPath);
    void addRecentFileOrProject(QMenu* menu, const QString& filePath, const std::function<void()>& callback);

    void saveProjectProperties();
    void loadProjectProperties();

    enum class OutputPane {
        Cargo,
        Application,
        Search
    };

    void loadSettings();
    void saveSettings();

    void saveSession();
    void loadSession();

    void openProject(const QString& path, bool isNew = false);
    void closeProject();

    void changeWindowTitle(const QString& filePath = QString());
    int findSource(const QString& filePath);
    void updateMenuState();

    Ui::MainWindow* ui;
    CargoManager* cargoManager;
    ApplicationManager* applicationManager;
    ProjectTree* projectTree;
    ProjectProperties* projectProperties;
    QString projectPath;
    TextEditor* editor = nullptr;
    AutoCompleter* completer;
    IssueModel* issueModel;
};
