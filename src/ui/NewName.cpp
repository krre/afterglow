#include "NewName.h"
#include "ui_NewName.h"
#include <QtWidgets>

NewName::NewName(const QString& title, QWidget* parent) : QDialog(parent) {
    setWindowTitle(title);
    resize(420, height());

    lineEdit = new QLineEdit;
    lineEdit->setFocus();

    auto gridLayout = new QGridLayout;
    gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

    auto label = new QLabel(tr("Name:"));
    gridLayout->addWidget(label, 0, 0, 1, 1);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addLayout(gridLayout);

    buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);
    setLayout(verticalLayout);

    connect(lineEdit, &QLineEdit::textChanged, this, &NewName::onTextChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    buttonBox->buttons().at(0)->setEnabled(false);
}

QString NewName::name() const {
    return lineEdit->text();
}

void NewName::onTextChanged(const QString& text) {
    buttonBox->buttons().at(0)->setEnabled(!text.isEmpty());
}
