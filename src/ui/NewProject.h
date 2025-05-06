#pragma once
#include "widgets/StandardDialog.h"
#include "process/CargoManager.h"

class QLineEdit;
class QComboBox;
class BrowseLayout;

class NewProject : public StandardDialog {
    Q_OBJECT
public:
    explicit NewProject(QWidget* parent = nullptr);

    QString path() const;
    CargoManager::Target target() const;

private slots:
    void adjustAcceptedButton();

private:
    QLineEdit* m_nameLineEdit = nullptr;
    BrowseLayout* m_directoryBrowseLayout = nullptr;
    QComboBox* m_targetComboBox = nullptr;
};
