#include "OverrideTab.h"
#include "RustInstaller.h"
#include "SetOverride.h"
#include "SelectableListView.h"
#include "ui/StringListModel.h"
#include <QtWidgets>

OverrideTab::OverrideTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    listView = new SelectableListView;
    listView->setModel(new StringListModel(this));

    setButton = new QPushButton(tr("Set..."));
    connect(setButton, &QPushButton::clicked, this, &OverrideTab::onSetClicked);

    unsetButton = new QPushButton(tr("Unset..."));
    connect(unsetButton, &QPushButton::clicked, this, &OverrideTab::onUnsetClicked);

    cleanupButton = new QPushButton(tr("Cleanup"));
    connect(cleanupButton, &QPushButton::clicked, this, &OverrideTab::onCleanupClicked);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(setButton);
    verticalLayout->addWidget(unsetButton);
    verticalLayout->addWidget(cleanupButton);
    verticalLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(listView);
    horizontalLayout->addLayout(verticalLayout);

    setLayout(horizontalLayout);
}

void OverrideTab::setWidgetsEnabled(bool enabled) {
    bool selected = listView->selectionModel()->selectedIndexes().count() && enabled;
    setButton->setEnabled(enabled);
    unsetButton->setEnabled(selected);
    cleanupButton->setEnabled(selected);
}

void OverrideTab::load() {
    listView->load("rustup override list");
    setWidgetsEnabled(true);
}

void OverrideTab::onSetClicked() {
    SetOverride setOverride(this);
    if (setOverride.exec() == QDialog::Rejected) return;

    QString directory = setOverride.directory();
    QString toolchain = setOverride.toolchain();

    if (!directory.isEmpty()) {
        rustupInstaller()->runCommand("rustup", QStringList("override") << "set" << toolchain, [this] {
            load();
        }, directory);
    }
}

void OverrideTab::onUnsetClicked() {
    int button = QMessageBox::question(this, tr("Unset Override"), tr("Override will be unsetted. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        for (const QString& override : listView->selectedRows()) {
            rustupInstaller()->runCommand("rustup", QStringList("override") << "unset" << "--path" << override.split('\t').at(0), [this] {
                load();
            });
        }
    }
}

void OverrideTab::onCleanupClicked() {
    rustupInstaller()->runCommand("rustup", { "override", "unset", "--nonexistent" }, [this] {
        load();
    });
}
