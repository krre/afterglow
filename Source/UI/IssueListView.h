#include <QListView>
#include <QStyledItemDelegate>

class IssueDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    IssueDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class IssueListView : public QListView {
    Q_OBJECT
public:
    explicit IssueListView(QWidget* parent = nullptr);

};
