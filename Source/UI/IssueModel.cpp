#include "IssueModel.h"
#include <QtCore>

IssueModel::IssueModel(QObject* parent) : QAbstractItemModel(parent) {
}

IssueModel::~IssueModel() {
}

void IssueModel::appendMessage(const QJsonObject& message) {
    QJsonObject messageObj = message["message"].toObject();
    QJsonObject spanObj = messageObj["spans"].toArray().at(0).toObject();

    Issue issue;

    issue.level = messageObj["level"].toString();
    issue.message = messageObj["message"].toString();
    issue.rendered = messageObj["rendered"].toString();
    issue.filename = spanObj["file_name"].toString();
    issue.line = spanObj["line_start"].toInt();
    issue.column = spanObj["column_start"].toInt();

    beginInsertRows(QModelIndex(), issues.count(), issues.count());
    issues.append(issue);
    endInsertRows();
}

void IssueModel::clear() {
    if (issues.isEmpty()) return;

    beginRemoveRows(QModelIndex(), 0, issues.count() - 1);
    issues.clear();
    endRemoveRows();
}

int IssueModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return 1;
}

QVariant IssueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    int row = index.row();

    if (row >= issues.count() || row < 0 || !index.isValid()) {
        return QVariant();
    }

    const Issue& issue = issues.at(row);
    Role issueRole = static_cast<Role>(role);

    if (issueRole == Role::Level) {
        return (issue.level);
    } else if (issueRole == Role::Message) {
        return issue.message;
    } else if (issueRole == Role::Rendered) {
        return issue.rendered;
    } else if (issueRole == Role::Filename) {
        return issue.filename;
    } else if (issueRole == Role::Line) {
        return issue.line;
    } else if (issueRole == Role::Column) {
        return issue.column;
    }

    return QVariant();
}

QModelIndex IssueModel::index(int row, int column, const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return createIndex(row, column);
}

QModelIndex IssueModel::parent(const QModelIndex& index) const {
    Q_UNUSED(index)
    return QModelIndex();
}

int IssueModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return issues.count();
}
