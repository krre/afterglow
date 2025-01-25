#include "IssueModel.h"
#include <QJsonObject>
#include <QJsonArray>

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

    beginInsertRows(QModelIndex(), m_issues.count(), m_issues.count());
    m_issues.append(issue);
    endInsertRows();

    emit countChanged(m_issues.count());
}

void IssueModel::clear() {
    if (m_issues.isEmpty()) return;

    beginRemoveRows(QModelIndex(), 0, m_issues.count() - 1);
    m_issues.clear();
    endRemoveRows();

    emit countChanged(0);
}

int IssueModel::columnCount(const QModelIndex& parent [[maybe_unused]]) const {
    return 1;
}

QVariant IssueModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    int row = index.row();

    if (row >= m_issues.count() || row < 0 || !index.isValid()) {
        return QVariant();
    }

    const Issue& issue = m_issues.at(row);
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

QModelIndex IssueModel::index(int row, int column, const QModelIndex& parent [[maybe_unused]]) const {
    return createIndex(row, column);
}

QModelIndex IssueModel::parent(const QModelIndex& index [[maybe_unused]]) const {
    return QModelIndex();
}

int IssueModel::rowCount(const QModelIndex& parent [[maybe_unused]]) const {
    return m_issues.count();
}
