#include "IssueModel.h"

IssueItem::IssueItem(const QList<Issue>& data, IssueItem* parentItem) {
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

 Issue IssueItem::getData(int column) const {
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
