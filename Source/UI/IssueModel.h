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

const int ROLES_COUNT = 6;

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssueModel(QObject* parent = nullptr);
    ~IssueModel();

    void appendMessage(const QJsonObject& message);

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    QList<Issue> issues;
};
