#include "GoToLine.h"
#include <QtWidgets>

GoToLine::GoToLine(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Go to Line"));

    auto label = new QLabel(tr("Line:"));

    auto validator = new QIntValidator(this);
    validator->setBottom(0);

    lineEdit = new QLineEdit;
    lineEdit->setValidator(validator);

    auto horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(label);
    horizontalLayout->addWidget(lineEdit);

    setContentLayout(horizontalLayout);

    setLayoutToFixedSize();

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(lineEdit, &QLineEdit::textChanged, this, &GoToLine::onTextChanged);

    lineEdit->setFocus();
}

int GoToLine::line() const {
    return lineEdit->text().toInt();
}

void GoToLine::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
