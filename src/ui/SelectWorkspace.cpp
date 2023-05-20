#include "SelectWorkspace.h"
#include "base/BrowseLayout.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

SelectWorkspace::SelectWorkspace(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Select Workspace"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Select directory for your Rust projects")));

    directoryBrowseLayout = new BrowseLayout;
    connect(directoryBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &SelectWorkspace::adjustAcceptedButton);

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Workspace:")), directoryBrowseLayout);

    verticalLayout->addLayout(formLayout);
    setContentLayout(verticalLayout);
    resizeToWidth(430);

    directoryBrowseLayout->lineEdit()->setText(Global::workspacePath());
    directoryBrowseLayout->lineEdit()->setFocus();
}

void SelectWorkspace::adjustAcceptedButton(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}

void SelectWorkspace::accept() {
    Settings::setValue("workspace", directoryBrowseLayout->lineEdit()->text());
    QDialog::accept();
}
