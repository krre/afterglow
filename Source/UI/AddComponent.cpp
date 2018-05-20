#include "AddComponent.h"
#include "ui_AddComponent.h"
#include "Core/Utils.h"
#include "StringListModel.h"

AddComponent::AddComponent(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AddComponent) {
    ui->setupUi(this);

    QStringList componentList = Utils::listFromConsole("rustup component list");
    for (int i = componentList.count() - 1; i >= 0; i--) {
        if (componentList.at(i).contains("(default)") || componentList.at(i).contains("(installed)")) {
            componentList.removeAt(i);
        }
    }

    QAbstractItemModel* model = new StringListModel(componentList, this);
    ui->listView->setModel(model);

    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

AddComponent::~AddComponent() {
    delete ui;
}
