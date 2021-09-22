#pragma once
#include "process/CargoManager.h"
#include <QDialog>

namespace Ui {
    class NewProject;
}

class CargoManager;

class NewProject : public QDialog {
    Q_OBJECT

public:
    explicit NewProject(QWidget* parent = nullptr);
    ~NewProject();
    QString getProjectPath() const { return projectPath; }
    CargoManager::ProjectTemplate getProjectTemplate() const { return projectTemplate; }

private slots:
    void on_pushButtonDirectory_clicked();
    void on_buttonBox_accepted();
    void adjustAcceptedButton();

private:
    Ui::NewProject* ui = nullptr;
    QString projectPath;
    CargoManager::ProjectTemplate projectTemplate;
};
