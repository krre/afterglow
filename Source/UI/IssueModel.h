#include <QAbstractItemModel>

struct Issue {
    enum class Level {
        Note,
        Info,
        Warning,
        Error
    };

    Level level;
    QString message;
    QString description;
    QString filename;
    int line;
    int column;
};

class IssueItem {
public:
    explicit IssueItem(const QList<Issue>& data, IssueItem* parentItem = nullptr);
    ~IssueItem();

    void appendChild(IssueItem* child);
    IssueItem* getChild(int row);
    int getChildCount() const;
    int getColumnCount() const;
    Issue getData(int column) const;
    int getRow() const;
    IssueItem* getParentItem();

private:
    QList<IssueItem*> childItems;
    QList<Issue> itemData;
    IssueItem* parentItem;
};

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssueModel(QObject* parent = nullptr);

signals:

};
