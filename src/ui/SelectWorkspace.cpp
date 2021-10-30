#include "SelectWorkspace.h"
#include "core/Const.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

SelectWorkspace::SelectWorkspace(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("Select Workspace"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Select directory for your Rust projects")));

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel(tr("Workspace:")));

    lineEdit = new QLineEdit;
    horizontalLayout->addWidget(lineEdit);

    auto browsePushButton = new QPushButton(tr("Browse..."));
    horizontalLayout->addWidget(browsePushButton);

    verticalLayout->addLayout(horizontalLayout);
    setContentLayout(verticalLayout);
    resizeToWidth(430);

    connect(browsePushButton, &QPushButton::clicked, this, &SelectWorkspace::onBrowseButtonClicked);
    connect(lineEdit, &QLineEdit::textChanged, this, &SelectWorkspace::adjustAcceptedButton);

    lineEdit->setText(Global::workspacePath());
    lineEdit->setFocus();
}

void SelectWorkspace::onBrowseButtonClicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);

    if (!dirPath.isEmpty()) {
        lineEdit->setText(dirPath);
    }
}

void SelectWorkspace::adjustAcceptedButton(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}

void SelectWorkspace::accept() {
    Settings::setValue("workspace", lineEdit->text());
    QDialog::accept();
}
