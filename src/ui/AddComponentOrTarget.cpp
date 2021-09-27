#include "AddComponentOrTarget.h"
#include "core/Utils.h"
#include "StringListModel.h"
#include <QtWidgets>

AddComponentOrTarget::AddComponentOrTarget(const QString& title, const QString& command, QWidget *parent) : QDialog(parent) {
    setWindowTitle(title);
    resize(400, 300);

    listView = new QListView;
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(listView);

    auto buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    verticalLayout->addWidget(buttonBox);

    setLayout(verticalLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    contextMenu = new QMenu(this);
    QAction* copyAction = contextMenu->addAction(tr("Copy"));
    connect(copyAction, &QAction::triggered, this, &AddComponentOrTarget::onCopyAction);

    connect(listView, &QListView::customContextMenuRequested, this, &AddComponentOrTarget::onCustomContextMenu);

    QStringList list = Utils::listFromConsole(command);

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

void AddComponentOrTarget::onCopyAction() {
    Utils::copySelectedRowsFromListViewToClipboard(listView);
}

void AddComponentOrTarget::onCustomContextMenu(const QPoint& point) {
    if (listView->indexAt(point).isValid()) {
        contextMenu->exec(listView->mapToGlobal(point));
    }
}

void AddComponentOrTarget::accept() {
    QModelIndexList indices = listView->selectionModel()->selectedIndexes();
    StringListModel* model = static_cast<StringListModel*>(listView->model());

    for (int i = 0; i < indices.count(); i++) {
        m_list.append(model->getData(indices.at(i).row()));
    }

    QDialog::accept();
}
