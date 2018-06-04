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

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit IssueModel(QObject* parent = nullptr);
    ~IssueModel();

    void appendMessage(const QJsonObject& message);

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

signals:

};
