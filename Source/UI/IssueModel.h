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
    QString rendered;
    QString filename;
    int line;
    int column;
};

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssueModel(QObject* parent = nullptr);
    ~IssueModel();

    void appendMessage(const QJsonObject& message);
    void clear();

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

private:
    QList<Issue> issues;
};
