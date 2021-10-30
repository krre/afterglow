#include "Dialog.h"
#include <QtWidgets>

Dialog::Dialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f) {
    m_buttonBox = new QDialogButtonBox;
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(m_buttonBox);
    setLayout(verticalLayout);
}

QDialogButtonBox* Dialog::buttonBox() const {
    return m_buttonBox;
}

void Dialog::setContentWidget(QWidget* contentWidget) {
    auto verticalLayout = static_cast<QVBoxLayout*>(layout());
    verticalLayout->insertWidget(0, contentWidget);
}

void Dialog::setContentLayout(QLayout* contentLayout, bool stretchAfter) {
    auto verticalLayout = static_cast<QVBoxLayout*>(layout());
    verticalLayout->insertLayout(0, contentLayout);

    if (stretchAfter) {
        verticalLayout->insertStretch(1);
    }
}

void Dialog::setLayoutToFixedSize() {
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void Dialog::resizeToWidth(int width) {
    resize(width, sizeHint().height());
}
