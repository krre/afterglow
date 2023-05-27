#include "ComponentTab.h"
#include "RustInstaller.h"
#include "SelectableListView.h"
#include "AddComponentOrTarget.h"
#include "ui/StringListModel.h"
#include "core/Utils.h"
#include <QtWidgets>

ComponentTab::ComponentTab(RustInstaller* rustupInstaller, QWidget* parent) : QWidget(parent), rustupInstaller(rustupInstaller) {
    listView = new SelectableListView;
    listView->setModel(new StringListModel(this));

    addButton = new QPushButton(tr("Add..."));
    connect(addButton, &QPushButton::clicked, this, &ComponentTab::onAddClicked);

    removeButton = new QPushButton(tr("Remove..."));
    connect(removeButton, &QPushButton::clicked, this, &ComponentTab::onRemoveClicked);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->setContentsMargins(0, -1, -1, -1);
    verticalLayout->addWidget(addButton);
    verticalLayout->addWidget(removeButton);
    verticalLayout->addStretch();

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(listView);
    horizontalLayout->addLayout(verticalLayout);

    setLayout(horizontalLayout);
    loadList();
}

void ComponentTab::setWidgetsEnabled(bool enabled) {
    bool selected = listView->selectionModel()->selectedIndexes().count() && enabled;
    addButton->setEnabled(enabled);
    removeButton->setEnabled(selected);
}

void ComponentTab::loadList() {
    listView->load("rustup component list", [] (QStringList& list) {
        Utils::defaultInstalledFilter(list);

        for (int i = list.count() - 1; i >= 0; i--) {
            if (list.at(i).left(8) == "rust-std") {
                list.removeAt(i);
            }
        }
    });
    setWidgetsEnabled(true);
}

void ComponentTab::onAddClicked() {
    AddComponentOrTarget addComponent(tr("Add Component"), "rustup component list", this);
    addComponent.exec();

    QStringList components = addComponent.list();

    if (components.count()) {
        rustupInstaller->cleanupTarget(components);
        rustupInstaller->runCommand("rustup", QStringList("component") << "add" << components, [this] {
            loadList();
        });
    }
}

void ComponentTab::onRemoveClicked() {
    int button = QMessageBox::question(this, tr("Remove Components"), tr("Components will be removed. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        QStringList components = listView->selectedRows();
        rustupInstaller->cleanupTarget(components);
        rustupInstaller->runCommand("rustup", QStringList("component") << "remove" << components, [this] {
            loadList();
        });
    }
}
