#include "IssueTreeView.h"
#include "IssueModel.h"

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

IssueTreeView::IssueTreeView(QWidget* parent) : QTreeView(parent) {
    setItemDelegate(new IssueDelegate(this));
}
