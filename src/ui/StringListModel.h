#pragma once
#include <QAbstractListModel>

class StringListModel : public QAbstractListModel {
    Q_OBJECT

public:
    StringListModel(QObject* parent = nullptr);
    StringListModel(const QStringList& strings = QStringList(), QObject* parent = nullptr);

    QString getData(int i) const { return stringList.at(i); }
    int getCount() const { return rowCount(); }

    void setStrings(const QStringList& strings);
    void clear();

protected:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role = Qt::EditRole) override;

    bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

private:
    QStringList stringList;
};
