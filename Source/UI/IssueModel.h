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
    explicit IssueItem(const QList<QVariant>& data, IssueItem* parentItem = nullptr);
    ~IssueItem();

    void appendChild(IssueItem* child);
    IssueItem* getChild(int row);
    int getChildCount() const;
    int getColumnCount() const;
    QVariant getData(int column) const;
    int getRow() const;
    IssueItem* getParentItem();

private:
    QList<IssueItem*> childItems;
    QList<QVariant> itemData;
    IssueItem* parentItem;
};

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssueModel(QObject* parent = nullptr);
    ~IssueModel();

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    IssueItem *rootItem;

signals:

};
