#include "SetOverride.h"
#include "core/Utils.h"
#include <QtWidgets>

SetOverride::SetOverride(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Set Override"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Directory:")));

    auto horizontalLayout = new QHBoxLayout;
    lineEdit = new QLineEdit;
    horizontalLayout->addWidget(lineEdit);

    auto browsePushButton = new QPushButton(tr("Browse..."));
    horizontalLayout->addWidget(browsePushButton);

    verticalLayout->addLayout(horizontalLayout);

    verticalLayout->addWidget(new QLabel(tr("Toolchain:")));

    comboBox = new QComboBox();
    verticalLayout->addWidget(comboBox, 0, Qt::AlignLeft);
    verticalLayout->addStretch();

    buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    verticalLayout->addWidget(buttonBox);

    setLayout(verticalLayout);
    adjustSize();
    resize(500, height());

    connect(browsePushButton, &QPushButton::clicked, this, &SetOverride::onBrowseButtonClicked);
    connect(lineEdit, &QLineEdit::textChanged, this, &SetOverride::onTextChanged);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    lineEdit->setFocus();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    QStringList list = Utils::listFromConsole("rustup toolchain list");

    for (const QString& toolchain : list) {
        comboBox->addItem(toolchain);
    }
}

QString SetOverride::directory() const {
    return lineEdit->text();
}

QString SetOverride::toolchain() const {
    return comboBox->currentText();
}

void SetOverride::onBrowseButtonClicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);

    if (!dirPath.isEmpty()) {
        lineEdit->setText(dirPath);
    }
}

void SetOverride::onTextChanged(const QString& text) {
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
