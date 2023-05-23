#include "InstallerListView.h"
#include <QtWidgets>

InstallerListView::InstallerListView() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(this, &QListView::customContextMenuRequested, this, &InstallerListView::onCustomContextMenu);
}

QStringList InstallerListView::selectedRows() const {
    QStringList result;
    QModelIndexList indices = selectionModel()->selectedIndexes();

    for (auto& index : indices) {
        result.append(model()->data(index).toString());
    }

    result.replaceInStrings(" (default)", "");
    result.replaceInStrings(" ", "");

    return result;
}

void InstallerListView::onCustomContextMenu(const QPoint& point) {
    if (!indexAt(point).isValid()) return;

    auto contextMenu = new QMenu(this);

    QAction* copyAction = contextMenu->addAction(tr("Copy"));
    connect(copyAction, &QAction::triggered, this, &InstallerListView::onCopyAction);

    contextMenu->exec(mapToGlobal(point));

}

void InstallerListView::onCopyAction() {
    QGuiApplication::clipboard()->setText(selectedRows().join('\n'));
}
