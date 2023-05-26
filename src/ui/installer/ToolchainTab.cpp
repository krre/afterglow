#include "ToolchainTab.h"
#include "RustInstaller.h"
#include "SelectableListView.h"
#include "InstallToolchain.h"
#include "ui/StringListModel.h"
#include "core/Utils.h"
#include <QtWidgets>

ToolchainTab::ToolchainTab(RustInstaller* rustupInstaller, QWidget* parent) : QWidget(parent), rustupInstaller(rustupInstaller) {
    listView = new SelectableListView;
    listView->setModel(new StringListModel(this));

    installButton = new QPushButton(tr("Install..."));
    connect(installButton, &QPushButton::clicked, this, &ToolchainTab::onInstallClicked);

    uninstallButton = new QPushButton(tr("Uninstall..."));
    uninstallButton->setEnabled(false);
    connect(uninstallButton, &QPushButton::clicked, this, &ToolchainTab::onUninstallClicked);

    updateButton = new QPushButton(tr("Update"));
    updateButton->setEnabled(false);
    connect(updateButton, &QPushButton::clicked, this, &ToolchainTab::onUpdateClicked);

    setDefaultButton = new QPushButton(tr("Set Default"));
    setDefaultButton->setEnabled(false);
    connect(setDefaultButton, &QPushButton::clicked, this, &ToolchainTab::onSetDefaultClicked);

    auto verticalLayout = new QVBoxLayout();
    verticalLayout->addWidget(installButton);
    verticalLayout->addWidget(uninstallButton);
    verticalLayout->addWidget(updateButton);
    verticalLayout->addWidget(setDefaultButton);
    verticalLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(listView);
    horizontalLayout->addLayout(verticalLayout);

    setLayout(horizontalLayout);

    loadList();
}

void ToolchainTab::setWidgetsEnabled(bool enabled) {
    bool selected = listView->selectionModel()->selectedIndexes().count() && enabled;
    installButton->setEnabled(enabled);
    uninstallButton->setEnabled(selected);
    updateButton->setEnabled(selected);
    setDefaultButton->setEnabled(selected);
}

void ToolchainTab::onInstallClicked() {
    InstallToolchain installToolchain(this);
    if (installToolchain.exec() == QDialog::Rejected) return;;

    QString toolchain = installToolchain.toolchain();

    if (!toolchain.isEmpty()) {
        rustupInstaller->runCommand("rustup", { "toolchain", "install", toolchain }, [this] {
            loadList();
        });
    }
}

void ToolchainTab::onUninstallClicked() {
    int button = QMessageBox::question(this, tr("Uninstall Toolchain"), tr("Toolchains will be uninstalled. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        rustupInstaller->runCommand("rustup", QStringList("toolchain") << "uninstall" << listView->selectedRows(), [this] {
            loadList();
        });
    }
}

void ToolchainTab::onUpdateClicked() {
    rustupInstaller->runCommand("rustup", QStringList("update") << listView->selectedRows());
}

void ToolchainTab::onSetDefaultClicked() {
    rustupInstaller->runCommand("rustup", QStringList("default") << listView->selectedRows().first(), [this] {
        loadList();
        emit defaultSetted();
    });
}

void ToolchainTab::loadList() {
    setWidgetsEnabled(false);
    listView->load("rustup toolchain list");
    setWidgetsEnabled(true);
}
