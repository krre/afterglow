#pragma once
#include "process/CargoManager.h"
#include <QDialog>

class QLineEdit;
class QDialogButtonBox;
class QComboBox;

class NewProject : public QDialog {
    Q_OBJECT
public:
    explicit NewProject(QWidget* parent = nullptr);

    QString path() const;
    CargoManager::ProjectTemplate projectTemplate() const;

private slots:
    void onBrowseButtonClicked();
    void adjustAcceptedButton();

private:
    QLineEdit* nameLineEdit = nullptr;
    QLineEdit* directoryLineEdit = nullptr;
    QComboBox* templateComboBox = nullptr;
    QDialogButtonBox* buttonBox = nullptr;
};
