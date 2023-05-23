#include "SelectableListView.h"
#include <QtWidgets>

SelectableListView::SelectableListView() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(this, &QListView::customContextMenuRequested, this, &SelectableListView::onCustomContextMenu);
}

QStringList SelectableListView::selectedRows() const {
    QStringList result;

    for (auto& index : selectionModel()->selectedIndexes()) {
        result.append(model()->data(index).toString());
    }

    result.replaceInStrings(" (default)", "");
    result.replaceInStrings(" ", "");

    return result;
}

void SelectableListView::onCustomContextMenu(const QPoint& point) {
    if (!indexAt(point).isValid()) return;

    auto contextMenu = new QMenu(this);

    QAction* copyAction = contextMenu->addAction(tr("Copy"));
    connect(copyAction, &QAction::triggered, this, &SelectableListView::onCopyAction);

    contextMenu->exec(mapToGlobal(point));
}

void SelectableListView::onCopyAction() {
    QGuiApplication::clipboard()->setText(selectedRows().join('\n'));
}
