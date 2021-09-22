#include "Rename.h"
#include <QtWidgets>

Rename::Rename(const QString& name, QWidget* parent) : QDialog(parent), m_name(name) {
    setWindowTitle(tr("Rename"));

    auto label = new QLabel;
    label->setText(tr("Rename %1?").arg(name));

    lineEdit = new QLineEdit;
    lineEdit->setText(name);
    lineEdit->setFocus();

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(label);
    verticalLayout->addWidget(lineEdit);

    buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    buttonBox->buttons().at(0)->setEnabled(false);

    verticalLayout->addWidget(buttonBox);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(verticalLayout);

    connect(lineEdit, &QLineEdit::textChanged, this, &Rename::onTextChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString Rename::name() const {
    return lineEdit->text();
}

void Rename::onTextChanged(const QString& text) {
    buttonBox->buttons().at(0)->setEnabled(!text.isEmpty() && text != m_name);
}
