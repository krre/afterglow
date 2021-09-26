#include "SelectWorkspace.h"
#include "core/Const.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

SelectWorkspace::SelectWorkspace(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Select Workspace"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(new QLabel(tr("Select directory for your Rust projects")));

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(new QLabel(tr("Workspace:")));

    lineEdit = new QLineEdit;
    horizontalLayout->addWidget(lineEdit);

    auto browsePushButton = new QPushButton(tr("Browse..."));

    horizontalLayout->addWidget(browsePushButton);

    verticalLayout->addLayout(horizontalLayout);

    buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);

    setLayout(verticalLayout);
    adjustSize();
    resize(430, height());

    connect(browsePushButton, &QPushButton::clicked, this, &SelectWorkspace::onBrowseButtonClicked);
    connect(lineEdit, &QLineEdit::textChanged, this, &SelectWorkspace::adjustAcceptedButton);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    lineEdit->setText(Global::workspacePath());
}

void SelectWorkspace::onBrowseButtonClicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);

    if (!dirPath.isEmpty()) {
        lineEdit->setText(dirPath);
    }
}

void SelectWorkspace::adjustAcceptedButton(const QString& text) {
    buttonBox->buttons().at(0)->setEnabled(!text.isEmpty());
}

void SelectWorkspace::accept() {
    Settings::setValue("workspace", lineEdit->text());
    QDialog::accept();
}
