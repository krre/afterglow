#pragma once
#include <QMainWindow>

class CargoManager;
class ProjectTreeView;

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
    void on_actionNewCargoProject_triggered();
    void on_actionNewRustFile_triggered();
    void on_actionExit_triggered();

    // Tools
    void on_actionOptions_triggered();

    // Help
    void on_actionAbout_triggered();

    // CargoManager
    void onProjectCreated(const QString& path);
    void onOutputMessage(const QString& message);

    void onFileRemoved(const QString& filePath);
    void addSourceTab(const QString& filePath);

private:

    enum class OutputPane {
        Cargo,
        Application,
        Search
    };

    void readSettings();
    void writeSettings();

    Ui::MainWindow* ui;
    CargoManager* cargoManager;
    ProjectTreeView* projectTreeView;
};
