#include "SetOverride.h"
#include "core/Utils.h"
#include "ui/widgets/BrowseLayout.h"
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFormLayout>

SetOverride::SetOverride(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Set Override"));

    m_directoryBrowseLayout = new BrowseLayout;
    connect(m_directoryBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &SetOverride::onTextChanged);

    m_toolchainComboBox = new QComboBox();

    auto formLayout = new QFormLayout;
    formLayout->addRow(tr("Directory:"), m_directoryBrowseLayout);
    formLayout->addRow(tr("Toolchain:"), m_toolchainComboBox);

    formLayout->itemAt(formLayout->indexOf(m_toolchainComboBox))->setAlignment(Qt::AlignLeft);

    setContentLayout(formLayout);
    resizeToWidth(500);

    m_directoryBrowseLayout->lineEdit()->setFocus();
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(false);

    QStringList list = Utils::runConsoleCommand("rustup toolchain list");

    for (const QString& toolchain : list) {
        m_toolchainComboBox->addItem(toolchain);
    }
}

QString SetOverride::directory() const {
    return m_directoryBrowseLayout->lineEdit()->text();
}

QString SetOverride::toolchain() const {
    return m_toolchainComboBox->currentText();
}

void SetOverride::onTextChanged(const QString& text) {
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(!text.isEmpty());
}
