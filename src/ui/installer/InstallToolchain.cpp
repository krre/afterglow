#include "InstallToolchain.h"
#include <QtWidgets>

InstallToolchain::InstallToolchain(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Install Toolchain"));
    
    m_dateLineEdit = new QLineEdit;
    
    m_channelComboBox = new QComboBox;
    m_channelComboBox->addItem("stable");
    m_channelComboBox->addItem("beta");
    m_channelComboBox->addItem("nightly");
    m_channelComboBox->addItem("version");
    
    m_channelLineEdit = new QLineEdit;
    m_channelLineEdit->setEnabled(false);

    auto channelHorizontalLayout = new QHBoxLayout;
    channelHorizontalLayout->setContentsMargins(-1, 0, -1, -1);
    channelHorizontalLayout->addWidget(m_channelComboBox);
    channelHorizontalLayout->addWidget(m_channelLineEdit);

    m_hostComboBox = new QComboBox;
    
    m_hostLineEdit = new QLineEdit;
    m_hostLineEdit->setEnabled(false);

    auto hostHorizontalLayout = new QHBoxLayout();
    hostHorizontalLayout->setContentsMargins(-1, 0, -1, -1);
    hostHorizontalLayout->addWidget(m_hostComboBox);
    hostHorizontalLayout->addWidget(m_hostLineEdit);

    auto formLayout = new QFormLayout;
    formLayout->addRow(tr("Channel:"), channelHorizontalLayout);
    formLayout->addRow(tr("Date:"), m_dateLineEdit);
    formLayout->addRow(tr("Host:"), hostHorizontalLayout);

    setContentLayout(formLayout);

    adjustSize();
    resize(530, height());
    
    connect(m_channelComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, qOverload<int>(&InstallToolchain::onChannelCurrentIndexChanged));
    connect(m_hostComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, qOverload<int>(&InstallToolchain::onHostCurrentIndexChanged));

    loadHosts();
}

QString InstallToolchain::toolchain() const {
    QString channel = m_channelComboBox->currentText();
    
    if (m_channelComboBox->currentIndex() == m_channelComboBox->count() - 1) {
        channel = m_channelLineEdit->text();
    }
    
    QString date = m_dateLineEdit->text();
    QString host = m_hostComboBox->currentText();

    if (m_hostComboBox->currentIndex() == m_hostComboBox->count() - 1) {
        host = m_hostLineEdit->text();
    }

    return channel + "-" + (!date.isEmpty() ? QString("%1-").arg(date) : "") + host;
}

void InstallToolchain::onChannelCurrentIndexChanged(int index) {
    m_channelLineEdit->setEnabled(index == m_channelComboBox->count() - 1);
    
    if (m_channelLineEdit->isEnabled()) {
        m_channelLineEdit->setFocus();
    }
}

void InstallToolchain::onHostCurrentIndexChanged(int index) {
    m_hostLineEdit->setEnabled(index == m_hostComboBox->count() - 1);
    
    if (m_hostLineEdit->isEnabled()) {
        m_hostLineEdit->setFocus();
    }
}

void InstallToolchain::loadHosts() {
#if defined(Q_OS_LINUX)
    m_hostComboBox->addItem("x86_64-unknown-linux-gnu");
    m_hostComboBox->addItem("i686-unknown-linux-gnu");
#elif defined(Q_OS_WIN)
    hostComboBox->addItem("x86_64-pc-windows-gnu");
    hostComboBox->addItem("x86_64-pc-windows-msvc");
    hostComboBox->addItem("i686-pc-windows-gnu");
    hostComboBox->addItem("i686-pc-windows-msvc");
#endif
    m_hostComboBox->addItem(tr("Custom"));
    onHostCurrentIndexChanged(0);
}
