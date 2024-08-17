#include "FileSystemProxyModel.h"
#include <QFileSystemModel>

FileSystemProxyModel::FileSystemProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {
}

bool FileSystemProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
    // If sorting by file names column
    if (sortColumn() == 0) {
        QFileSystemModel *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
        bool asc = sortOrder() == Qt::AscendingOrder ? true : false;

        QFileInfo leftFileInfo  = fsm->fileInfo(left);
        QFileInfo rightFileInfo = fsm->fileInfo(right);

        // If DotAndDot move in the beginning
        if (sourceModel()->data(left).toString() == "..")  return asc;
        if (sourceModel()->data(right).toString() == "..") return !asc;

        // Move dirs upper
        if (!leftFileInfo.isDir() && rightFileInfo.isDir()) {
            return !asc;
        }

        if (leftFileInfo.isDir() && !rightFileInfo.isDir()) {
            return asc;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
