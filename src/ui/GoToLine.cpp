#include "GoToLine.h"
#include <QtWidgets>

GoToLine::GoToLine(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Go to Line"));

    auto label = new QLabel(tr("Line:"));

    auto validator = new QIntValidator(this);
    validator->setBottom(0);
    
    m_lineEdit = new QLineEdit;
    m_lineEdit->setValidator(validator);

    auto horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(label);
    horizontalLayout->addWidget(m_lineEdit);

    setContentLayout(horizontalLayout);

    setLayoutToFixedSize();

    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &GoToLine::onTextChanged);
    
    m_lineEdit->setFocus();
}

int GoToLine::line() const {
    return m_lineEdit->text().toInt();
}

void GoToLine::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
