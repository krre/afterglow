#pragma once
#include <QAbstractListModel>

class StringListModel : public QAbstractListModel {
    Q_OBJECT

public:
    StringListModel(QObject* parent = 0);
    StringListModel(const QStringList& strings = QStringList(), QObject* parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

    void setStrings(const QStringList& strings);
    void clear();

private:
    QStringList stringList;
};
