#include "Rename.h"
#include <QtWidgets>

Rename::Rename(const QString& path, QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Rename"));

    QFileInfo fi(path);
    m_name = fi.fileName();

    auto label = new QLabel;
    label->setText(QString("Rename %1 %2?").arg(fi.isDir() ? "directory" : "file", m_name));

    auto lineEdit = new QLineEdit;
    lineEdit->setText(m_name);
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
    connect(buttonBox, &QDialogButtonBox::rejected, this, &Rename::onRejected);
}

const QString& Rename::name() const {
    return m_name;
}

void Rename::onRejected() {
    m_name = QString();
    reject();
}

void Rename::onTextChanged(const QString& text) {
    buttonBox->buttons().at(0)->setEnabled(!text.isEmpty() && text != m_name);
    m_name = text;
}
