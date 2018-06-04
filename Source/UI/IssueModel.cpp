#include "IssueModel.h"
#include <QtCore>

IssueModel::IssueModel(QObject* parent) : QAbstractItemModel(parent) {
}

IssueModel::~IssueModel() {
}

void IssueModel::appendMessage(const QJsonObject& message) {
    qDebug() << message;
}

int IssueModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant IssueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags IssueModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IssueModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

QModelIndex IssueModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    return QModelIndex();
}

QModelIndex IssueModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    return QModelIndex();
}

int IssueModel::rowCount(const QModelIndex &parent) const {
    return 0;
}
