#include "IssueListView.h"
#include "IssueModel.h"

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

IssueTreeView::IssueTreeView(QWidget* parent) : QListView(parent) {
    setItemDelegate(new IssueDelegate(this));
}
