#include "IssueListView.h"
#include "IssueModel.h"

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

void IssueDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyledItemDelegate::paint(painter, option, index);
}

QSize IssueDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QStyledItemDelegate::sizeHint(option, index);
}

IssueListView::IssueListView(QWidget* parent) : QListView(parent) {
    setItemDelegate(new IssueDelegate(this));
}
