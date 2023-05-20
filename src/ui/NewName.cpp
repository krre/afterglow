#include "NewName.h"
#include <QtWidgets>

NewName::NewName(const QString& title, QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(title);

    lineEdit = new QLineEdit;

    auto gridLayout = new QGridLayout;
    gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

    auto label = new QLabel(tr("Name:"));
    gridLayout->addWidget(label, 0, 0, 1, 1);

    setContentLayout(gridLayout);
    resizeToWidth(400);

    connect(lineEdit, &QLineEdit::textChanged, this, &NewName::onTextChanged);

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    lineEdit->setFocus();
}

QString NewName::name() const {
    return lineEdit->text();
}

void NewName::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
