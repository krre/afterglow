#include "Rename.h"
#include <QtWidgets>

Rename::Rename(const QString& name, QWidget* parent) : StandardDialog(parent), m_name(name) {
    setWindowTitle(tr("Rename"));

    auto label = new QLabel;
    label->setText(tr("Rename %1?").arg(name));

    lineEdit = new QLineEdit;
    lineEdit->setText(name);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(lineEdit);
    setContentLayout(verticalLayout);
    resizeToWidth(300);

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(lineEdit, &QLineEdit::textChanged, this, &Rename::onTextChanged);

    lineEdit->setFocus();
}

QString Rename::name() const {
    return lineEdit->text();
}

void Rename::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty() && text != m_name);
}
