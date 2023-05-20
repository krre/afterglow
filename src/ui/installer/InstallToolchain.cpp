#include "InstallToolchain.h"
#include <QtWidgets>

InstallToolchain::InstallToolchain(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Install Toolchain"));

    auto gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Channel:")), 0, 0, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Date:")), 1, 0, 1, 1);

    dateLineEdit = new QLineEdit;
    gridLayout->addWidget(dateLineEdit, 1, 1, 1, 1);

    auto channelHorizontalLayout = new QHBoxLayout;
    channelHorizontalLayout->setContentsMargins(-1, 0, -1, -1);

    channelComboBox = new QComboBox;
    channelComboBox->addItem("stable");
    channelComboBox->addItem("beta");
    channelComboBox->addItem("nightly");
    channelComboBox->addItem("version");

    channelHorizontalLayout->addWidget(channelComboBox);

    channelLineEdit = new QLineEdit;
    channelLineEdit->setEnabled(false);

    channelHorizontalLayout->addWidget(channelLineEdit);
    gridLayout->addLayout(channelHorizontalLayout, 0, 1, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Host:")), 2, 0, 1, 1);

    auto hostHorizontalLayout = new QHBoxLayout();
    hostHorizontalLayout->setContentsMargins(-1, 0, -1, -1);
    hostComboBox = new QComboBox;

    hostHorizontalLayout->addWidget(hostComboBox);

    hostLineEdit = new QLineEdit;
    hostLineEdit->setEnabled(false);

    hostHorizontalLayout->addWidget(hostLineEdit);
    gridLayout->addLayout(hostHorizontalLayout, 2, 1, 1, 1);

    setContentLayout(gridLayout);

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
