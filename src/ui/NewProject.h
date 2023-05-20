#pragma once
#include "base/StandardDialog.h"
#include "process/CargoManager.h"

class QLineEdit;
class QComboBox;
class BrowseLayout;

class NewProject : public StandardDialog {
    Q_OBJECT
public:
    explicit NewProject(QWidget* parent = nullptr);

    QString path() const;
    CargoManager::ProjectTemplate projectTemplate() const;

private slots:
    void adjustAcceptedButton();

private:
    QLineEdit* nameLineEdit = nullptr;
    BrowseLayout* directoryBrowseLayout = nullptr;
    QComboBox* templateComboBox = nullptr;
};
