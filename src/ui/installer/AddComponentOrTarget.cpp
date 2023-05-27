#include "AddComponentOrTarget.h"
#include "SelectableListView.h"
#include "core/Utils.h"
#include "ui/StringListModel.h"
#include <QtWidgets>

AddComponentOrTarget::AddComponentOrTarget(const QString& title, const QString& command, QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(title);
    resize(400, 300);

    QStringList list = Utils::runConsoleCommand(command);

    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).contains("(default)") || list.at(i).contains("(installed)")
                || (command.contains("component") && list.at(i).left(8) == "rust-std")) {
            list.removeAt(i);
        }
    }

    listView = new SelectableListView(list);
    setContentWidget(listView);

    if (list.count()) {
        listView->setCurrentIndex(listView->model()->index(0, 0));
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
