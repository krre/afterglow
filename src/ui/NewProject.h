#pragma once
#include "base/Dialog.h"
#include "process/CargoManager.h"

class QLineEdit;
class QComboBox;
class BrowseLineEdit;

class NewProject : public Dialog {
    Q_OBJECT
public:
    explicit NewProject(QWidget* parent = nullptr);

    QString path() const;
    CargoManager::ProjectTemplate projectTemplate() const;

private slots:
    void adjustAcceptedButton();

private:
    QLineEdit* nameLineEdit = nullptr;
    BrowseLineEdit* directoryBrowseLineEdit = nullptr;
    QComboBox* templateComboBox = nullptr;
};
