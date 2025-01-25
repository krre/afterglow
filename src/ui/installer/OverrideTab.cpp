#include "OverrideTab.h"
#include "RustInstaller.h"
#include "SetOverride.h"
#include "SelectableListView.h"
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

OverrideTab::OverrideTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    m_listView = new SelectableListView;

    m_setButton = new QPushButton(tr("Set..."));
    connect(m_setButton, &QPushButton::clicked, this, &OverrideTab::onSetClicked);

    m_unsetButton = new QPushButton(tr("Unset..."));
    connect(m_unsetButton, &QPushButton::clicked, this, &OverrideTab::onUnsetClicked);

    m_cleanupButton = new QPushButton(tr("Cleanup"));
    connect(m_cleanupButton, &QPushButton::clicked, this, &OverrideTab::onCleanupClicked);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(m_setButton);
    verticalLayout->addWidget(m_unsetButton);
    verticalLayout->addWidget(m_cleanupButton);
    verticalLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_listView);
    horizontalLayout->addLayout(verticalLayout);

    setLayout(horizontalLayout);
}

void OverrideTab::setWidgetsEnabled(bool enabled) {
    bool selected = m_listView->selectionModel()->selectedIndexes().count() && enabled;
    m_setButton->setEnabled(enabled);
    m_unsetButton->setEnabled(selected);
    m_cleanupButton->setEnabled(selected);
}

void OverrideTab::load() {
    m_listView->load("rustup override list");
    setWidgetsEnabled(true);
}

CoTask OverrideTab::onSetClicked() {
    SetOverride setOverride(this);
    if (setOverride.exec() == QDialog::Rejected) co_return;

    QString directory = setOverride.directory();
    QString toolchain = setOverride.toolchain();

    if (!directory.isEmpty()) {
        co_await rustupInstaller()->runCommand("rustup", QStringList("override") << "set" << toolchain, directory);
        load();
    }
}

CoTask OverrideTab::onUnsetClicked() {
    int button = QMessageBox::question(this, tr("Unset Override"), tr("Override will be unsetted. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        for (const QString& override : m_listView->selectedRows()) {
            co_await rustupInstaller()->runCommand("rustup", QStringList("override") << "unset" << "--path" << override.split('\t').at(0));
            load();
        }
    }
}

CoTask OverrideTab::onCleanupClicked() {
    co_await rustupInstaller()->runCommand("rustup", QStringList("override") << "unset" << "--nonexistent");
    load();
}
