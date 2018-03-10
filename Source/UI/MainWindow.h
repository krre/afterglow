#pragma once
#include <QMainWindow>

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
    void on_actionExit_triggered();

    // View
    void on_actionShowSidebar_toggled(bool checked=false);
    void on_actionShowOutput_toggled(bool checked=false);

    // Help
    void on_actionAbout_triggered();

private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow* ui;
};
