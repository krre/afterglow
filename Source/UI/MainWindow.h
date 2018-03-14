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
    void on_actionOpenProject_triggered();
    void on_actionCloseProject_triggered();

    void on_actionNewFile_triggered();
    void on_actionOpenFile_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();

    void on_actionClose_triggered();
    void on_actionCloseAll_triggered();
    void on_actionCloseOther_triggered();

    void on_actionExit_triggered();

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

    void onFileRemoved(const QString& filePath);
    void addSourceTab(const QString& filePath);

    // Editor
    void onDocumentModified(Editor* editor);

private:

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
