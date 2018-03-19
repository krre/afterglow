#pragma once
#include <QMainWindow>

class CargoManager;
class ProjectTreeView;
class Editor;

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

    void on_actionOpenFileProject_triggered();
    void on_actionCloseProject_triggered();

    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionSaveAll_triggered();
    void on_actionClearMenuRecentFiles_triggered();

    void on_actionClose_triggered();
    void on_actionCloseAll_triggered();
    void on_actionCloseOther_triggered();

    void on_actionExit_triggered();

    // Edit
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionSelectAll_triggered();

    // Cargo
    void on_actionBuild_triggered();
    void on_actionRun_triggered();

    // Tools
    void on_actionOptions_triggered();

    // Help
    void on_actionAbout_triggered();

    // TabWidet
    void on_tabWidgetSource_tabCloseRequested(int index);
    void on_tabWidgetSource_currentChanged(int index);
    void on_toolButtonCargoClear_clicked();
    void on_toolButtonAppClear_clicked();

    // CargoManager
    void onProjectCreated(const QString& path);
    void onOutputMessage(const QString& message);

    // ProjectTreeView
    void onFileCreated(const QString& filePath);
    void onFileRemoved(const QString& filePath);

    void addSourceTab(const QString& filePath);
    void addNewFile(const QString& filePath);

    // Editor
    void onDocumentModified(Editor* editor);

private:
    void addRecentFile(const QString& filePath);

    enum class OutputPane {
        Cargo,
        Application,
        Search
    };

    void readSettings();
    void writeSettings();

    void saveSession();
    void restoreSession();

    void openProject(const QString& path);
    void closeProject();

    void changeWindowTitle(const QString& filePath = QString());
    int findSource(const QString& filePath);
    void updateMenuState();

    Ui::MainWindow* ui;
    CargoManager* cargoManager;
    ProjectTreeView* projectTreeView;
    QString projectPath;

};
