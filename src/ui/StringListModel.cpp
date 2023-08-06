#include "StringListModel.h"

StringListModel::StringListModel(const QStringList& rows, QObject* parent) : QAbstractListModel(parent), m_rows(rows) {

}

StringListModel::StringListModel(QObject* parent) : QAbstractListModel(parent) {

}

std::optional<QString> StringListModel::find(const QString& str) const {
    auto index = m_rows.indexOf(str);
    return index == -1 ? std::nullopt : std::optional(m_rows[index]);
}

int StringListModel::rowCount(const QModelIndex& parent [[maybe_unused]]) const {
    return m_rows.count();
}

QVariant StringListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_rows.size()) {
        return QVariant();
    }

    return role == Qt::DisplayRole || role == Qt::EditRole ? m_rows.at(index.row()) : QVariant();
}

QVariant StringListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    return orientation == Qt::Horizontal ? QString("Column %1").arg(section) : QString("Row %1").arg(section);
}

Qt::ItemFlags StringListModel::flags(const QModelIndex& index) const {
    return !index.isValid() ? Qt::ItemIsEnabled : QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool StringListModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        m_rows.replace(index.row(), value.toString());
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

bool StringListModel::insertRows(int position, int rows, const QModelIndex& parent [[maybe_unused]]) {
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        m_rows.insert(position, "");
    }

    endInsertRows();
    return true;
}

bool StringListModel::removeRows(int position, int rows, const QModelIndex& parent [[maybe_unused]]) {
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        m_rows.removeAt(position);
    }

    endRemoveRows();
    return true;
}

void StringListModel::setRows(const QStringList& strings) {
    beginResetModel();
    m_rows = strings;
    endResetModel();
}

void StringListModel::clear() {
    beginResetModel();
    m_rows.clear();
    endResetModel();
}
