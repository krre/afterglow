#include "GoToLine.h"
#include <QtWidgets>

GoToLine::GoToLine(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Go to Line"));

    auto horizontalLayout = new QHBoxLayout();
    auto label = new QLabel(tr("Line:"));
    horizontalLayout->addWidget(label);

    auto validator = new QIntValidator(this);
    validator->setBottom(0);

    lineEdit = new QLineEdit;
    lineEdit->setValidator(validator);
    horizontalLayout->addWidget(lineEdit);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addLayout(horizontalLayout);

    buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    verticalLayout->addWidget(buttonBox);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(verticalLayout);

    connect(lineEdit, &QLineEdit::textChanged, this, &GoToLine::onTextChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int GoToLine::line() const {
    return lineEdit->text().toInt();
}

void GoToLine::onTextChanged(const QString& text) {
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
