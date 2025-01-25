#include "ComponentTab.h"
#include "RustInstaller.h"
#include "SelectableListView.h"
#include "AddComponentOrTarget.h"
#include "core/Utils.h"
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>

ComponentTab::ComponentTab(RustInstaller* rustupInstaller, QWidget* parent) : InstallerTab(rustupInstaller, parent) {
    m_listView = new SelectableListView;

    m_addButton = new QPushButton(tr("Add..."));
    connect(m_addButton, &QPushButton::clicked, this, &ComponentTab::onAddClicked);

    m_removeButton = new QPushButton(tr("Remove..."));
    connect(m_removeButton, &QPushButton::clicked, this, &ComponentTab::onRemoveClicked);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->setContentsMargins(0, -1, -1, -1);
    verticalLayout->addWidget(m_addButton);
    verticalLayout->addWidget(m_removeButton);
    verticalLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(m_listView);
    horizontalLayout->addLayout(verticalLayout);

    setLayout(horizontalLayout);
}

void ComponentTab::setWidgetsEnabled(bool enabled) {
    bool selected = m_listView->selectionModel()->selectedIndexes().count() && enabled;
    m_addButton->setEnabled(enabled);
    m_removeButton->setEnabled(selected);
}

void ComponentTab::load() {
    m_listView->load("rustup component list", [] (QStringList& list) {
        Utils::defaultInstalledFilter(list);

        for (int i = list.count() - 1; i >= 0; i--) {
            if (list.at(i).left(8) == "rust-std") {
                list.removeAt(i);
            }
        }
    });

    setWidgetsEnabled(true);
}

CoTask ComponentTab::onAddClicked() {
    AddComponentOrTarget addComponent(tr("Add Component"), "rustup component list", this);
    addComponent.exec();

    QStringList components = addComponent.list();

    if (components.count()) {
        rustupInstaller()->cleanupTarget(components);
        co_await rustupInstaller()->runCommand("rustup", QStringList("component") << "add" << components);
        load();
    }
}

CoTask ComponentTab::onRemoveClicked() {
    int button = QMessageBox::question(this, tr("Remove Components"), tr("Components will be removed. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        QStringList components = m_listView->selectedRows();
        rustupInstaller()->cleanupTarget(components);
        co_await rustupInstaller()->runCommand("rustup", QStringList("component") << "remove" << components);
        load();
    }
}
