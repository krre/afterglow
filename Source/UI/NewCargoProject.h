#pragma once
#include "Cargo/CargoManager.h"
#include <QDialog>

namespace Ui {
    class NewCargoProject;
}

class CargoManager;

class NewCargoProject : public QDialog {
    Q_OBJECT

public:
    explicit NewCargoProject(QWidget* parent = 0);
    ~NewCargoProject();
    QString getProjectPath() const { return projectPath; }
    CargoManager::ProjectTemplate getProjectTemplate() const { return projectTemplate; }

private slots:
    void on_pushButtonDirectory_clicked();
    void on_buttonBox_accepted();
    void adjustAcceptedButton();

private:
    Ui::NewCargoProject* ui;
    QString projectPath;
    CargoManager::ProjectTemplate projectTemplate;
};
