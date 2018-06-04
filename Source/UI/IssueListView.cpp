#include "IssueListView.h"
#include "IssueModel.h"

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

IssueListView::IssueListView(QWidget* parent) : QListView(parent) {
    setItemDelegate(new IssueDelegate(this));
}
