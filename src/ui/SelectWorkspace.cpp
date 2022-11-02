#include "SelectWorkspace.h"
#include "base/BrowseLineEdit.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

SelectWorkspace::SelectWorkspace(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("Select Workspace"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Select directory for your Rust projects")));

    directoryBrowseLineEdit = new BrowseLineEdit;
    connect(directoryBrowseLineEdit->lineEdit(), &QLineEdit::textChanged, this, &SelectWorkspace::adjustAcceptedButton);

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Workspace:")), directoryBrowseLineEdit);

    verticalLayout->addLayout(formLayout);
    setContentLayout(verticalLayout);
    resizeToWidth(430);

    directoryBrowseLineEdit->lineEdit()->setText(Global::workspacePath());
    directoryBrowseLineEdit->lineEdit()->setFocus();
}

void SelectWorkspace::adjustAcceptedButton(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}

void SelectWorkspace::accept() {
    Settings::setValue("workspace", directoryBrowseLineEdit->lineEdit()->text());
    QDialog::accept();
}
