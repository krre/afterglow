#pragma once
#include <QSortFilterProxyModel>

class FileSystemProxyModel : public QSortFilterProxyModel {
public:
    FileSystemProxyModel(QObject* parent = nullptr);

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
};
