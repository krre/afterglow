#pragma once
#include <QAbstractItemModel>

struct Issue {
    QString level;
    QString message;
    QString rendered;
    QString filename;
    int line;
    int column;
};

class IssueModel : public QAbstractItemModel {
    Q_OBJECT
public:

    enum class Role {
        Level = Qt::UserRole,
        Message,
        Rendered,
        Filename,
        Line,
        Column
    };

    explicit IssueModel(QObject* parent = nullptr);
    ~IssueModel();

    void appendMessage(const QJsonObject& message);
    void clear();

    QVariant data(const QModelIndex& index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

signals:
    void countChanged(int count);

private:
    QList<Issue> issues;
};
