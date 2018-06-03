#include <QListView>
#include <QStyledItemDelegate>

class IssueListView : public QListView {
    Q_OBJECT
public:
    explicit IssueListView(QWidget* parent = nullptr);

};

class IssueDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    IssueDelegate(QObject* parent = nullptr);

};
