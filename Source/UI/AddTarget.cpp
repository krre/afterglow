#include "AddTarget.h"
#include "ui_AddTarget.h"
#include "Core/Utils.h"
#include "StringListModel.h"
#include <QtCore>

AddTarget::AddTarget(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AddTarget) {
    ui->setupUi(this);

    QStringList list = Utils::getListFromConsole("rustup component list");
    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).contains("(default)") || list.at(i).contains("(installed)")) {
            list.removeAt(i);
        }
    }

    QAbstractItemModel* model = new StringListModel(list, this);
    ui->listView->setModel(model);

    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

AddTarget::~AddTarget() {
    delete ui;
}

void AddTarget::on_buttonBox_accepted() {
    QModelIndexList indices = ui->listView->selectionModel()->selectedIndexes();
    StringListModel* model = static_cast<StringListModel*>(ui->listView->model());

    for (int i = 0; i < indices.count(); i++) {
        targets.append(model->data(indices.at(i), Qt::DisplayRole).toString());
    }
}
