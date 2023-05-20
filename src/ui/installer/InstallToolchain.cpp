#include "InstallToolchain.h"
#include <QtWidgets>

InstallToolchain::InstallToolchain(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Install Toolchain"));

    dateLineEdit = new QLineEdit;

    channelComboBox = new QComboBox;
    channelComboBox->addItem("stable");
    channelComboBox->addItem("beta");
    channelComboBox->addItem("nightly");
    channelComboBox->addItem("version");

    channelLineEdit = new QLineEdit;
    channelLineEdit->setEnabled(false);

    auto channelHorizontalLayout = new QHBoxLayout;
    channelHorizontalLayout->setContentsMargins(-1, 0, -1, -1);
    channelHorizontalLayout->addWidget(channelComboBox);
    channelHorizontalLayout->addWidget(channelLineEdit);

    hostComboBox = new QComboBox;

    hostLineEdit = new QLineEdit;
    hostLineEdit->setEnabled(false);

    auto hostHorizontalLayout = new QHBoxLayout();
    hostHorizontalLayout->setContentsMargins(-1, 0, -1, -1);
    hostHorizontalLayout->addWidget(hostComboBox);
    hostHorizontalLayout->addWidget(hostLineEdit);

    auto formLayout = new QFormLayout;
    formLayout->addRow(tr("Channel:"), channelHorizontalLayout);
    formLayout->addRow(tr("Date:"), dateLineEdit);
    formLayout->addRow(tr("Host:"), hostHorizontalLayout);

    setContentLayout(formLayout);

    adjustSize();
    resize(530, height());

    connect(channelComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, qOverload<int>(&InstallToolchain::onChannelCurrentIndexChanged));
    connect(hostComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, qOverload<int>(&InstallToolchain::onHostCurrentIndexChanged));

    loadHosts();
}

QString InstallToolchain::toolchain() const {
    QString channel = channelComboBox->currentText();

    if (channelComboBox->currentIndex() == channelComboBox->count() - 1) {
        channel = channelLineEdit->text();
    }

    QString date = dateLineEdit->text();
    QString host = hostComboBox->currentText();

    if (hostComboBox->currentIndex() == hostComboBox->count() - 1) {
        host = hostLineEdit->text();
    }

    return channel + "-" + (!date.isEmpty() ? QString("%1-").arg(date) : "") + host;
}

void InstallToolchain::onChannelCurrentIndexChanged(int index) {
    channelLineEdit->setEnabled(index == channelComboBox->count() - 1);

    if (channelLineEdit->isEnabled()) {
        channelLineEdit->setFocus();
    }
}

void InstallToolchain::onHostCurrentIndexChanged(int index) {
    hostLineEdit->setEnabled(index == hostComboBox->count() - 1);

    if (hostLineEdit->isEnabled()) {
        hostLineEdit->setFocus();
    }
}

void InstallToolchain::loadHosts() {
#if defined(Q_OS_LINUX)
    hostComboBox->addItem("x86_64-unknown-linux-gnu");
    hostComboBox->addItem("i686-unknown-linux-gnu");
#elif defined(Q_OS_WIN)
    hostComboBox->addItem("x86_64-pc-windows-gnu");
    hostComboBox->addItem("x86_64-pc-windows-msvc");
    hostComboBox->addItem("i686-pc-windows-gnu");
    hostComboBox->addItem("i686-pc-windows-msvc");
#endif
    hostComboBox->addItem(tr("Custom"));
    onHostCurrentIndexChanged(0);
}
