#include <QListView>
#include <QStyledItemDelegate>

class IssueDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    IssueDelegate(QObject* parent = nullptr);

};

class IssueTreeView : public QListView {
    Q_OBJECT
public:
    explicit IssueTreeView(QWidget* parent = nullptr);

};
