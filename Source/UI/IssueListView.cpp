#include "IssueListView.h"
#include "IssueModel.h"

IssueListView::IssueListView(QWidget* parent) : QListView(parent) {
    setItemDelegate(new IssueDelegate(this));
}

IssueDelegate::IssueDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}
