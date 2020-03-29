#include "StringListModel.h"

StringListModel::StringListModel(QObject* parent) : QAbstractListModel(parent) {

}

StringListModel::StringListModel(const QStringList& strings, QObject* parent)
    : QAbstractListModel(parent), stringList(strings) {}

int StringListModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return stringList.count();
}

QVariant StringListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= stringList.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return stringList.at(index.row());
    } else {
        return QVariant();
    }
}

QVariant StringListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        return QString("Column %1").arg(section);
    } else {
        return QString("Row %1").arg(section);
    }
}

Qt::ItemFlags StringListModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool StringListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.isValid() && role == Qt::EditRole) {

        stringList.replace(index.row(), value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool StringListModel::insertRows(int position, int rows, const QModelIndex& parent) {
    Q_UNUSED(parent)

    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        stringList.insert(position, "");
    }

    endInsertRows();
    return true;
}

bool StringListModel::removeRows(int position, int rows, const QModelIndex& parent) {
    Q_UNUSED(parent)

    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        stringList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

void StringListModel::setStrings(const QStringList& strings) {
    beginResetModel();
    stringList = strings;
    endResetModel();
}

void StringListModel::clear() {
    beginResetModel();
    stringList.clear();
    endResetModel();
}
