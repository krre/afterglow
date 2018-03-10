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
    // Help
    void on_actionAbout_triggered();

private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow* ui;
};
