#include "IssueModel.h"
#include <QtCore>

IssueItem::IssueItem(const QList<QVariant>& data, IssueItem* parentItem) {
    this->parentItem = parentItem;
    itemData = data;
}

IssueItem::~IssueItem() {
    qDeleteAll(childItems);
}

void IssueItem::appendChild(IssueItem* child) {
    childItems.append(child);
}

IssueItem* IssueItem::getChild(int row) {
     return childItems.value(row);
 }

 int IssueItem::getChildCount() const {
     return childItems.count();
 }

 int IssueItem::getColumnCount() const {
     return itemData.count();
 }

 QVariant IssueItem::getData(int column) const {
     return itemData.value(column);
 }

 IssueItem* IssueItem::getParentItem() {
     return parentItem;
 }

 int IssueItem::getRow() const {
     if (parentItem) {
         return parentItem->childItems.indexOf(const_cast<IssueItem*>(this));
     }

     return 0;
 }


IssueModel::IssueModel(QObject* parent) : QAbstractItemModel(parent) {
}

IssueModel::~IssueModel() {
    delete rootItem;
}

void IssueModel::appendMessage(const QJsonObject& message) {
    qDebug() << message;
}

int IssueModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return static_cast<IssueItem*>(parent.internalPointer())->getColumnCount();
    } else {
        return rootItem->getColumnCount();
    }
}

QVariant IssueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    IssueItem* item = static_cast<IssueItem*>(index.internalPointer());

    return item->getData(index.column());
}

Qt::ItemFlags IssueModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return 0;

    return QAbstractItemModel::flags(index);
}

QVariant IssueModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->getData(section);
    }

    return QVariant();
}

QModelIndex IssueModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    IssueItem* parentItem;

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<IssueItem*>(parent.internalPointer());
    }

    IssueItem* childItem = parentItem->getChild(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex IssueModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    IssueItem* childItem = static_cast<IssueItem*>(index.internalPointer());
    IssueItem* parentItem = childItem->getParentItem();

    if (parentItem == rootItem) return QModelIndex();

    return createIndex(parentItem->getRow(), 0, parentItem);
}

int IssueModel::rowCount(const QModelIndex &parent) const {
    IssueItem* parentItem;
    if (parent.column() > 0) return 0;

    if (!parent.isValid()) {
        parentItem = rootItem;
    } else {
        parentItem = static_cast<IssueItem*>(parent.internalPointer());
    }

    return parentItem->getChildCount();
}
