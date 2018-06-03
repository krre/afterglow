#include <QTreeView>
#include <QStyledItemDelegate>

class IssueDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    IssueDelegate(QObject* parent = nullptr);

};

class IssueTreeView : public QTreeView {
    Q_OBJECT
public:
    explicit IssueTreeView(QWidget* parent = nullptr);

};
