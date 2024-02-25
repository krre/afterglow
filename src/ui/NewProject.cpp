#include "NewProject.h"
#include "base/BrowseLayout.h"
#include "core/Global.h"
#include "process/CargoManager.h"
#include <QtWidgets>

NewProject::NewProject(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("New Project"));

    m_nameLineEdit = new QLineEdit;

    m_directoryBrowseLayout = new BrowseLayout;
    m_directoryBrowseLayout->lineEdit()->setText(Global::workspacePath());

    m_targetComboBox = new QComboBox;
    m_targetComboBox->addItem(tr("Binary"));
    m_targetComboBox->addItem(tr("Library"));

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Name:")), m_nameLineEdit);
    formLayout->addRow(new QLabel(tr("Directory:")), m_directoryBrowseLayout);
    formLayout->addRow(new QLabel(tr("Target:")), m_targetComboBox);

    formLayout->itemAt(formLayout->indexOf(m_targetComboBox))->setAlignment(Qt::AlignLeft);

    setContentLayout(formLayout);
    resizeToWidth(400);

    connect(m_nameLineEdit, &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);
    connect(m_directoryBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &NewProject::adjustAcceptedButton);

    adjustAcceptedButton();
    m_nameLineEdit->setFocus();
}

QString NewProject::path() const {
    return m_directoryBrowseLayout->lineEdit()->text() + "/" + m_nameLineEdit->text();
}

CargoManager::Target NewProject::target() const {
    return static_cast<CargoManager::Target>(m_targetComboBox->currentIndex());
}

void NewProject::adjustAcceptedButton() {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!m_nameLineEdit->text().isEmpty() && !m_directoryBrowseLayout->lineEdit()->text().isEmpty());
}
