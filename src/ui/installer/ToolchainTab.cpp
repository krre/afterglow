#include "ToolchainTab.h"
#include "RustInstaller.h"
#include "SelectableListView.h"
#include "InstallToolchain.h"
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>

ToolchainTab::ToolchainTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    listView = new SelectableListView;

    m_installButton = new QPushButton(tr("Install..."));
    connect(m_installButton, &QPushButton::clicked, this, &ToolchainTab::onInstallClicked);

    m_uninstallButton = new QPushButton(tr("Uninstall..."));
    m_uninstallButton->setEnabled(false);
    connect(m_uninstallButton, &QPushButton::clicked, this, &ToolchainTab::onUninstallClicked);

    m_updateButton = new QPushButton(tr("Update"));
    m_updateButton->setEnabled(false);
    connect(m_updateButton, &QPushButton::clicked, this, &ToolchainTab::onUpdateClicked);

    m_setDefaultButton = new QPushButton(tr("Set Default"));
    m_setDefaultButton->setEnabled(false);
    connect(m_setDefaultButton, &QPushButton::clicked, this, &ToolchainTab::onSetDefaultClicked);

    auto verticalLayout = new QVBoxLayout();
    verticalLayout->addWidget(m_installButton);
    verticalLayout->addWidget(m_uninstallButton);
    verticalLayout->addWidget(m_updateButton);
    verticalLayout->addWidget(m_setDefaultButton);
    verticalLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(listView);
    horizontalLayout->addLayout(verticalLayout);

    setLayout(horizontalLayout);
}

void ToolchainTab::setWidgetsEnabled(bool enabled) {
    bool selected = listView->selectionModel()->selectedIndexes().count() && enabled;
    m_installButton->setEnabled(enabled);
    m_uninstallButton->setEnabled(selected);
    m_updateButton->setEnabled(selected);
    m_setDefaultButton->setEnabled(selected);
}

CoTask ToolchainTab::onInstallClicked() {
    InstallToolchain installToolchain(this);
    if (installToolchain.exec() == QDialog::Rejected) co_return;;

    QString toolchain = installToolchain.toolchain();

    if (!toolchain.isEmpty()) {
        co_await rustupInstaller()->runCommand("rustup", QStringList("toolchain") <<  "install" << toolchain);
        load();
    }
}

CoTask ToolchainTab::onUninstallClicked() {
    int button = QMessageBox::question(this, tr("Uninstall Toolchain"), tr("Toolchains will be uninstalled. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        co_await rustupInstaller()->runCommand("rustup", QStringList("toolchain") << "uninstall" << listView->selectedRows());
        load();
    }
}

void ToolchainTab::onUpdateClicked() {
    rustupInstaller()->runCommand("rustup", QStringList("update") << listView->selectedRows());
}

CoTask ToolchainTab::onSetDefaultClicked() {
    co_await rustupInstaller()->runCommand("rustup", QStringList("default") << listView->selectedRows().constFirst());
    load();
    emit defaultSetted();
}

void ToolchainTab::load() {
    setWidgetsEnabled(false);
    listView->load("rustup toolchain list");
    setWidgetsEnabled(true);
}
