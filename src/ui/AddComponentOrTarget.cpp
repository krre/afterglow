#include "AddComponentOrTarget.h"
#include "ui_AddComponentOrTarget.h"
#include "core/Utils.h"
#include "StringListModel.h"
#include <QtWidgets>

AddComponentOrTarget::AddComponentOrTarget(const QString& title, const QString& command, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AddComponentOrTarget) {
    ui->setupUi(this);
    setWindowTitle(title);

    contextMenu = new QMenu(this);
    QAction* copyAction = contextMenu->addAction(tr("Copy"));
    connect(copyAction, &QAction::triggered, this, &AddComponentOrTarget::onCopyAction);

    connect(ui->listView, &QListView::customContextMenuRequested, this, &AddComponentOrTarget::onCustomContextMenu);

    QStringList list = Utils::getListFromConsole(command);
    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).contains("(default)") || list.at(i).contains("(installed)")
                || (command.contains("component") && list.at(i).left(8) == "rust-std")) {
            list.removeAt(i);
        }
    }

    QAbstractItemModel* model = new StringListModel(list, this);
    ui->listView->setModel(model);

    if (model->rowCount()) {
        ui->listView->setCurrentIndex(model->index(0, 0));
    }
}

AddComponentOrTarget::~AddComponentOrTarget() {
    delete ui;
}

void AddComponentOrTarget::on_buttonBox_accepted() {
    QModelIndexList indices = ui->listView->selectionModel()->selectedIndexes();
    StringListModel* model = static_cast<StringListModel*>(ui->listView->model());

    for (int i = 0; i < indices.count(); i++) {
        list.append(model->getData(indices.at(i).row()));
    }
}

void AddComponentOrTarget::onCopyAction() {
    Utils::copySelectedRowsFromListViewToClipboard(ui->listView);
}

void AddComponentOrTarget::onCustomContextMenu(const QPoint& point) {
    if (ui->listView->indexAt(point).isValid()) {
        contextMenu->exec(ui->listView->mapToGlobal(point));
    }
}
