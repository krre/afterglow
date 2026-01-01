#include "RustupTab.h"
#include "RustInstaller.h"
#include "ui/widgets/BrowseLayout.h"
#include "core/Constants.h"
#include "core/Settings.h"
#include "core/Utils.h"
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFormLayout>
#include <coroutine>

RustupTab::RustupTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    auto rustupHomeBrowseLayout = new BrowseLayout;
    m_rustupHomeLineEdit = rustupHomeBrowseLayout->lineEdit();
    m_rustupHomeLineEdit->setText(qEnvironmentVariable(Const::Environment::RustupHome));

    auto cargoHomeBrowseLayout = new BrowseLayout;
    m_cargoHomeLineEdit = cargoHomeBrowseLayout->lineEdit();
    m_cargoHomeLineEdit->setText(qEnvironmentVariable(Const::Environment::CargoHome));

    auto envLayout = new QFormLayout;
    envLayout->addRow(new QLabel(QString(Const::Environment::RustupHome) + ":"), rustupHomeBrowseLayout);
    envLayout->addRow(new QLabel(QString(Const::Environment::CargoHome) + ":"), cargoHomeBrowseLayout);

    auto groupBox = new QGroupBox(tr("Environment Variables"));
    groupBox->setLayout(envLayout);

    m_versionLineEdit = new QLineEdit;
    m_versionLineEdit->setReadOnly(true);

    auto versionLayout = new QFormLayout;
    versionLayout->addRow(tr("Version:"), m_versionLineEdit);

    m_downloadButton = new QPushButton(tr("Download"));
    connect(m_downloadButton, &QPushButton::clicked, this, &RustupTab::onDownloadClicked);

    m_updateButton = new QPushButton(tr("Update"));
    connect(m_updateButton, &QPushButton::clicked, this, &RustupTab::onUpdateClicked);

    m_updateAllButton = new QPushButton(tr("Update All"));
    connect(m_updateAllButton, &QPushButton::clicked, this, &RustupTab::onUpdateAllClicked);

    m_uninstallButton = new QPushButton(tr("Uninstall..."));
    connect(m_uninstallButton, &QPushButton::clicked, this, &RustupTab::onUninstallClicked);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_downloadButton);
    buttonLayout->addWidget(m_updateButton);
    buttonLayout->addWidget(m_updateAllButton);
    buttonLayout->addWidget(m_uninstallButton);
    buttonLayout->addStretch();

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(groupBox);
    verticalLayout->addLayout(versionLayout);
    verticalLayout->addLayout(buttonLayout);
    verticalLayout->addStretch();

    setLayout(verticalLayout);
}

void RustupTab::setWidgetsEnabled(bool enabled) {
    m_downloadButton->setEnabled(enabled);
    m_updateButton->setEnabled(enabled);
    m_updateAllButton->setEnabled(enabled);
    m_uninstallButton->setEnabled(enabled);
}

void RustupTab::load() {
    const auto rows = Utils::runConsoleCommand("rustup -V");

    for (const QString& row : rows) {
        if (row.left(6) == "rustup") {
            m_versionLineEdit->setText(row);
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
    rustupInstaller()->downloadInstaller();
}

CoTask RustupTab::onUpdateClicked() {
    co_await rustupInstaller()->runCommand("rustup", QStringList("self") << "update");
    load();
}

CoTask RustupTab::onUpdateAllClicked() {
    co_await rustupInstaller()->runCommand("rustup", QStringList("update"));
    load();
}

void RustupTab::onUninstallClicked() {
    int button = QMessageBox::question(this, tr("Uninstall Rust"), tr("Rust will be uninstalled. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);

    if (button == QMessageBox::Ok) {
        rustupInstaller()->runCommand("rustup", { "self", "uninstall", "-y" });
    }
}
