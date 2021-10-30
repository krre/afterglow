#include "NewProject.h"
#include "core/Global.h"
#include "process/CargoManager.h"
#include <QtWidgets>

NewProject::NewProject(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("New Project"));

    auto gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Name:")), 0, 0, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Directory:")), 1, 0, 1, 1);

    nameLineEdit = new QLineEdit;
    gridLayout->addWidget(nameLineEdit, 0, 1, 1, 1);

    auto horizontalLayout = new QHBoxLayout;

    directoryLineEdit = new QLineEdit(Global::workspacePath());
    horizontalLayout->addWidget(directoryLineEdit);

    auto browsePushButton = new QPushButton(tr("Browse..."));
    horizontalLayout->addWidget(browsePushButton);

    gridLayout->addLayout(horizontalLayout, 1, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Template:")), 2, 0, 1, 1);

    templateComboBox = new QComboBox;
    templateComboBox->addItem(tr("Binary"));
    templateComboBox->addItem(tr("Library"));

    gridLayout->addWidget(templateComboBox, 2, 1, 1, 1, Qt::AlignLeft);

    setContentLayout(gridLayout);
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
