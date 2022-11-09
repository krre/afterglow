#include "SetOverride.h"
#include "core/Utils.h"
#include "ui/base/BrowseLayout.h"
#include <QtWidgets>

SetOverride::SetOverride(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("Set Override"));

    directoryBrowseLayout = new BrowseLayout;
    connect(directoryBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &SetOverride::onTextChanged);

    toolchainComboBox = new QComboBox();

    auto formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Directory:")), directoryBrowseLayout);
    formLayout->addRow(new QLabel(tr("Toolchain:")), toolchainComboBox);
    formLayout->itemAt(1, QFormLayout::FieldRole)->setAlignment(Qt::AlignLeft);

    setContentLayout(formLayout);
    resizeToWidth(500);

    directoryBrowseLayout->lineEdit()->setFocus();
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);

    QStringList list = Utils::listFromConsole("rustup toolchain list");

    for (const QString& toolchain : list) {
        toolchainComboBox->addItem(toolchain);
    }
}

QString SetOverride::directory() const {
    return directoryBrowseLayout->lineEdit()->text();
}

QString SetOverride::toolchain() const {
    return toolchainComboBox->currentText();
}

void SetOverride::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
