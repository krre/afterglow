#include "TargetTab.h"
#include "RustInstaller.h"
#include "SelectableListView.h"
#include "AddComponentOrTarget.h"
#include "core/Utils.h"
#include <QtWidgets>

TargetTab::TargetTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    m_listView = new SelectableListView;
    
    m_addButton = new QPushButton(tr("Add..."));
    connect(m_addButton, &QPushButton::clicked, this, &TargetTab::onAddClicked);
    
    m_removeButton = new QPushButton(tr("Remove..."));
    connect(m_removeButton, &QPushButton::clicked, this, &TargetTab::onRemoveClicked);

    auto buttonLayout = new QVBoxLayout;
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_listView);
    horizontalLayout->addLayout(buttonLayout);

    setLayout(horizontalLayout);
}

void TargetTab::setWidgetsEnabled(bool enabled) {
    bool selected = m_listView->selectionModel()->selectedIndexes().count() && enabled;
    m_addButton->setEnabled(enabled);
    m_removeButton->setEnabled(selected);
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
        rustupInstaller()->runCommand("rustup", QStringList("target") << "remove" << m_listView->selectedRows(), [this] {
            load();
            rustupInstaller()->loadComponents();
        });
    }
}

void TargetTab::load() {
    m_listView->load("rustup target list", [] (QStringList& list) { Utils::defaultInstalledFilter(list); });
    setWidgetsEnabled(true);
    m_defaultTarget = m_listView->findDefault();
}

QString TargetTab::defaultTarget() const {
    return m_defaultTarget;
}
