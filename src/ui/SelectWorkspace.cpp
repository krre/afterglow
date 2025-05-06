#include "SelectWorkspace.h"
#include "widgets/BrowseLayout.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFormLayout>

SelectWorkspace::SelectWorkspace(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Select Workspace"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Select directory for your Rust projects")));

    m_directoryBrowseLayout = new BrowseLayout;
    connect(m_directoryBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &SelectWorkspace::adjustAcceptedButton);

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Workspace:")), m_directoryBrowseLayout);

    verticalLayout->addLayout(formLayout);
    setContentLayout(verticalLayout);
    resizeToWidth(430);

    m_directoryBrowseLayout->lineEdit()->setText(Global::workspacePath());
    m_directoryBrowseLayout->lineEdit()->setFocus();
}

void SelectWorkspace::adjustAcceptedButton(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}

void SelectWorkspace::accept() {
    Settings::setValue("workspace", m_directoryBrowseLayout->lineEdit()->text());
    QDialog::accept();
}
