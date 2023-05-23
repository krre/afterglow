#include "AddComponentOrTarget.h"
#include "InstallerListView.h"
#include "core/Utils.h"
#include "ui/StringListModel.h"
#include <QtWidgets>

AddComponentOrTarget::AddComponentOrTarget(const QString& title, const QString& command, QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(title);
    resize(400, 300);

    listView = new InstallerListView;
    setContentWidget(listView);

    QStringList list = Utils::runConsoleCommand(command);

    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).contains("(default)") || list.at(i).contains("(installed)")
                || (command.contains("component") && list.at(i).left(8) == "rust-std")) {
            list.removeAt(i);
        }
    }

    QAbstractItemModel* model = new StringListModel(list, this);
    listView->setModel(model);

    if (model->rowCount()) {
        listView->setCurrentIndex(model->index(0, 0));
    }
}

void AddComponentOrTarget::accept() {
    QModelIndexList indices = listView->selectionModel()->selectedIndexes();
    StringListModel* model = static_cast<StringListModel*>(listView->model());

    for (auto index : indices) {
        m_list.append(model->data(index).toString());
    }

    QDialog::accept();
}
