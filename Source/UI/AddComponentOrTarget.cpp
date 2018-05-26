#include "AddComponentOrTarget.h"
#include "ui_AddComponentOrTarget.h"
#include "Core/Utils.h"
#include "StringListModel.h"
#include <QtCore>

AddComponentOrTarget::AddComponentOrTarget(const QString& title, const QString& command, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AddComponentOrTarget) {
    ui->setupUi(this);
    setWindowTitle(title);

    QStringList list = Utils::getListFromConsole(command);
    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).contains("(default)") || list.at(i).contains("(installed)")) {
            list.removeAt(i);
        }
    }

    QAbstractItemModel* model = new StringListModel(list, this);
    ui->listView->setModel(model);

    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

AddComponentOrTarget::~AddComponentOrTarget() {
    delete ui;
}

void AddComponentOrTarget::on_buttonBox_accepted() {
    QModelIndexList indices = ui->listView->selectionModel()->selectedIndexes();
    StringListModel* model = static_cast<StringListModel*>(ui->listView->model());

    for (int i = 0; i < indices.count(); i++) {
        list.append(model->data(indices.at(i), Qt::DisplayRole).toString());
    }
}
