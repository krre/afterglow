#include "TargetTab.h"
#include "RustInstaller.h"
#include "SelectableListView.h"
#include "AddComponentOrTarget.h"
#include "core/Utils.h"
#include <QtWidgets>

TargetTab::TargetTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    listView = new SelectableListView;

    addButton = new QPushButton(tr("Add..."));
    connect(addButton, &QPushButton::clicked, this, &TargetTab::onAddClicked);

    removeButton = new QPushButton(tr("Remove..."));
    connect(removeButton, &QPushButton::clicked, this, &TargetTab::onRemoveClicked);

    auto buttonLayout = new QVBoxLayout;

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(listView);
    horizontalLayout->addLayout(buttonLayout);

    setLayout(horizontalLayout);
}

void TargetTab::setWidgetsEnabled(bool enabled) {
    bool selected = listView->selectionModel()->selectedIndexes().count() && enabled;
    addButton->setEnabled(enabled);
    removeButton->setEnabled(selected);
}

void TargetTab::onAddClicked() {
    AddComponentOrTarget addTarget(tr("Add Target"), "rustup target list", this);
    addTarget.exec();

    QStringList targets = addTarget.list();

    if (targets.count()) {
        rustupInstaller()->runCommand("rustup", QStringList("target") << "add" << targets, [this] {
            load();
            rustupInstaller()->loadComponents();
        });
    }
}

void TargetTab::onRemoveClicked() {
    int button = QMessageBox::question(this, tr("Remove Targets"), tr("Targets will be removed. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        rustupInstaller()->runCommand("rustup", QStringList("target") << "remove" << listView->selectedRows(), [this] {
            load();
            rustupInstaller()->loadComponents();
        });
    }
}

void TargetTab::load() {
    listView->load("rustup target list", [] (QStringList& list) { Utils::defaultInstalledFilter(list); });
    setWidgetsEnabled(true);
    m_defaultTarget = listView->findDefault();
}

QString TargetTab::defaultTarget() const {
    return m_defaultTarget;
}
