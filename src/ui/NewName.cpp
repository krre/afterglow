#include "NewName.h"
#include <QtWidgets>

NewName::NewName(const QString& title, QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(title);
    
    m_lineEdit = new QLineEdit;
    connect(m_lineEdit, &QLineEdit::textChanged, this, &NewName::onTextChanged);

    auto formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), m_lineEdit);

    setContentLayout(formLayout);
    resizeToWidth(400);

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    m_lineEdit->setFocus();
}

QString NewName::name() const {
    return m_lineEdit->text();
}

void NewName::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
