#include "RustupTab.h"
#include "RustInstaller.h"
#include "ui/base/BrowseLayout.h"
#include "core/Constants.h"
#include "core/Settings.h"
#include "core/Utils.h"
#include <QtWidgets>

RustupTab::RustupTab(RustInstaller* rustupInstaller, QWidget* parent) : QWidget(parent), rustupInstaller(rustupInstaller) {
    auto rustupHomeBrowseLayout = new BrowseLayout;
    rustupHomeLineEdit = rustupHomeBrowseLayout->lineEdit();
    rustupHomeLineEdit->setText(qEnvironmentVariable(Const::Environment::RustupHome));

    auto cargoHomeBrowseLayout = new BrowseLayout;
    cargoHomeLineEdit = cargoHomeBrowseLayout->lineEdit();
    cargoHomeLineEdit->setText(qEnvironmentVariable(Const::Environment::CargoHome));

    auto envLayout = new QFormLayout;
    envLayout->addRow(new QLabel("RUSTUP_HOME:"), rustupHomeBrowseLayout);
    envLayout->addRow(new QLabel("RUSTUP_HOME:"), cargoHomeBrowseLayout);

    auto groupBox = new QGroupBox(tr("Environment Variables"));
    groupBox->setLayout(envLayout);

    versionLineEdit = new QLineEdit;
    versionLineEdit->setReadOnly(true);

    auto versionLayout = new QFormLayout;
    versionLayout->addRow(tr("Version:"), versionLineEdit);

    downloadButton = new QPushButton(tr("Download"));
    connect(downloadButton, &QPushButton::clicked, this, &RustupTab::onDownloadClicked);

    updateButton = new QPushButton(tr("Update"));
    connect(updateButton, &QPushButton::clicked, this, &RustupTab::onUpdateClicked);

    updateAllButton = new QPushButton(tr("Update All"));
    connect(updateAllButton, &QPushButton::clicked, this, &RustupTab::onUpdateAllClicked);

    uninstallButton = new QPushButton(tr("Uninstall..."));
    connect(uninstallButton, &QPushButton::clicked, this, &RustupTab::onUninstallClicked);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(downloadButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(updateAllButton);
    buttonLayout->addWidget(uninstallButton);
    buttonLayout->addStretch();

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(groupBox);
    verticalLayout->addLayout(versionLayout);
    verticalLayout->addLayout(buttonLayout);
    verticalLayout->addStretch();

    setLayout(verticalLayout);
    loadVersion();
}

void RustupTab::setWidgetsEnabled(bool enabled) {
    downloadButton->setEnabled(enabled);
    updateButton->setEnabled(enabled);
    updateAllButton->setEnabled(enabled);
    uninstallButton->setEnabled(enabled);
}

void RustupTab::loadVersion() {
    for (const QString& row : Utils::runConsoleCommand("rustup show")) {
        if (row.left(5) == "rustc") {
            versionLineEdit->setText(row);
            break;
        }
    }
}

void RustupTab::onRustupHomeChanged(const QString& text) {
    Settings::setValue("environment.rustupHome", text);

}

void RustupTab::onCargoHomeChanged(const QString& text) {
    Settings::setValue("environment.cargoHome", text);
}

void RustupTab::onDownloadClicked() {
    rustupInstaller->downloadInstaller();
}

void RustupTab::onUpdateClicked() {
    rustupInstaller->runCommand("rustup", { "self", "update" }, [this] {
        loadVersion();
    });
}

void RustupTab::onUpdateAllClicked() {
    rustupInstaller->runCommand("rustup", { "update" }, [this] {
        loadVersion();
    });
}

void RustupTab::onUninstallClicked() {
    int button = QMessageBox::question(this, tr("Uninstall Rust"), tr("Rust will be uninstalled. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);

    if (button == QMessageBox::Ok) {
        rustupInstaller->runCommand("rustup", { "self", "uninstall", "-y" });
    }
}
