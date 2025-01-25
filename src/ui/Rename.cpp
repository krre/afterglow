#include "Rename.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>

Rename::Rename(const QString& name, QWidget* parent) : StandardDialog(parent), m_name(name) {
    setWindowTitle(tr("Rename"));

    auto label = new QLabel;
    label->setText(tr("Rename %1?").arg(name));

    m_lineEdit = new QLineEdit;
    m_lineEdit->setText(name);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(m_lineEdit);
    setContentLayout(verticalLayout);
    resizeToWidth(300);

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &Rename::onTextChanged);

    m_lineEdit->setFocus();
}

QString Rename::name() const {
    return m_lineEdit->text();
}

void Rename::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty() && text != m_name);
}
