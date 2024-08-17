#include "SelectableListView.h"
#include "ui/StringListModel.h"
#include "core/Utils.h"
#include <QtWidgets>

SelectableListView::SelectableListView(const QStringList& rows) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setModel(new StringListModel(rows, this));

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

QString SelectableListView::findDefault() const {
    StringListModel* model = static_cast<StringListModel*>(QListView::model());
    auto str = model->find("(default)");
    return str == std::nullopt ? QString() : str->replace(" (default)", "");
}

void SelectableListView::load(const QString& command, const std::function<void (QStringList&)>& filter) {
    QStringList list = Utils::runConsoleCommand(command);

    if (list.count() == 1 && list.first().left(2) == "no") {
        list.removeFirst();
    }

    if (filter) {
        filter(list);
    }

    if (list.count()) {
        StringListModel* model = static_cast<StringListModel*>(QListView::model());
        model->setRows(list);
        setCurrentIndex(model->index(0, 0));
    }
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
