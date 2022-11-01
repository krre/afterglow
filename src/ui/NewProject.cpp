#include "NewProject.h"
#include "core/Global.h"
#include "process/CargoManager.h"
#include <QtWidgets>

NewProject::NewProject(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("New Project"));

    nameLineEdit = new QLineEdit;

    directoryLineEdit = new QLineEdit(Global::workspacePath());
    auto browsePushButton = new QPushButton(tr("Browse..."));

    auto directoryLayout = new QHBoxLayout;
    directoryLayout->addWidget(directoryLineEdit);
    directoryLayout->addWidget(browsePushButton);

    templateComboBox = new QComboBox;
    templateComboBox->addItem(tr("Binary"));
    templateComboBox->addItem(tr("Library"));

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Name:")), nameLineEdit);
    formLayout->addRow(new QLabel(tr("Directory:")), directoryLayout);
    formLayout->addRow(new QLabel(tr("Template:")), templateComboBox);
    formLayout->itemAt(2, QFormLayout::FieldRole)->setAlignment(Qt::AlignLeft);

    setContentLayout(formLayout);
    resizeToWidth(400);

    connect(browsePushButton, &QPushButton::clicked, this, &NewProject::onBrowseButtonClicked);
    connect(nameLineEdit, &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);
    connect(directoryLineEdit, &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);

    adjustAcceptedButton();
    nameLineEdit->setFocus();
}

QString NewProject::path() const {
    return directoryLineEdit->text() + "/" + nameLineEdit->text();
}

CargoManager::ProjectTemplate NewProject::projectTemplate() const {
    return static_cast<CargoManager::ProjectTemplate>(templateComboBox->currentIndex());
}

void NewProject::onBrowseButtonClicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);

    if (!dirPath.isEmpty()) {
        directoryLineEdit->setText(dirPath);
    }
}

void NewProject::adjustAcceptedButton() {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!nameLineEdit->text().isEmpty() && !directoryLineEdit->text().isEmpty());
}
