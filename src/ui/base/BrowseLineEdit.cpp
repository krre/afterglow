#include "BrowseLineEdit.h"
#include <QtWidgets>

BrowseLineEdit::BrowseLineEdit() {
    m_lineEdit = new QLineEdit;

    auto pushButton = new QPushButton(tr("Browse..."));
    connect(pushButton, &QPushButton::clicked, this, &BrowseLineEdit::onClicked);

    addWidget(m_lineEdit);
    addWidget(pushButton);
}

QLineEdit* BrowseLineEdit::lineEdit() const {
    return m_lineEdit;
}

void BrowseLineEdit::onClicked() {
    QString dirPath = QFileDialog::getExistingDirectory();

    if (!dirPath.isEmpty()) {
        m_lineEdit->setText(dirPath);
    }
}
