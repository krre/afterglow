#include "NewProject.h"
#include "base/BrowseLineEdit.h"
#include "core/Global.h"
#include "process/CargoManager.h"
#include <QtWidgets>

NewProject::NewProject(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("New Project"));

    nameLineEdit = new QLineEdit;

    directoryBrowseLineEdit = new BrowseLineEdit;
    directoryBrowseLineEdit->lineEdit()->setText(Global::workspacePath());

    templateComboBox = new QComboBox;
    templateComboBox->addItem(tr("Binary"));
    templateComboBox->addItem(tr("Library"));

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Name:")), nameLineEdit);
    formLayout->addRow(new QLabel(tr("Directory:")), directoryBrowseLineEdit);
    formLayout->addRow(new QLabel(tr("Template:")), templateComboBox);
    formLayout->itemAt(2, QFormLayout::FieldRole)->setAlignment(Qt::AlignLeft);

    setContentLayout(formLayout);
    resizeToWidth(400);

    connect(nameLineEdit, &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);
    connect(directoryBrowseLineEdit->lineEdit(), &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);

    adjustAcceptedButton();
    nameLineEdit->setFocus();
}

QString NewProject::path() const {
    return directoryBrowseLineEdit->lineEdit()->text() + "/" + nameLineEdit->text();
}

CargoManager::ProjectTemplate NewProject::projectTemplate() const {
    return static_cast<CargoManager::ProjectTemplate>(templateComboBox->currentIndex());
}

void NewProject::adjustAcceptedButton() {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!nameLineEdit->text().isEmpty() && !directoryBrowseLineEdit->lineEdit()->text().isEmpty());
}
