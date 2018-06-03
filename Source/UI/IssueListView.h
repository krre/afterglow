#include <QListView>
#include <QStyledItemDelegate>

class IssueDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    IssueDelegate(QObject* parent = nullptr);

};

class IssueListView : public QListView {
    Q_OBJECT
public:
    explicit IssueListView(QWidget* parent = nullptr);

};
