#include "NewName.h"
#include <QtWidgets>

NewName::NewName(const QString& title, QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(title);

    lineEdit = new QLineEdit;
    connect(lineEdit, &QLineEdit::textChanged, this, &NewName::onTextChanged);

    auto formLayout = new QFormLayout;
    formLayout->addRow(tr("Name:"), lineEdit);

    setContentLayout(formLayout);
    resizeToWidth(400);

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    lineEdit->setFocus();
}

QString NewName::name() const {
    return lineEdit->text();
}

void NewName::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
