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

    QString level = messageObj["level"].toString();
    if (level == "note") {
        issue.level = Issue::Level::Note;
    } else if (level == "info") {
        issue.level = Issue::Level::Info;
    } else if (level == "warning") {
        issue.level = Issue::Level::Warning;
    } else if (level == "error") {
        issue.level = Issue::Level::Error;
    }

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

    if (index.row() >= issues.count() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        const Issue& issue = issues.at(index.row());

        if (index.column() == 0) {
            return static_cast<int>(issue.level);
        } else if (index.column() == 1) {
            return issue.message;
        } else if (index.column() == 2) {
            return issue.rendered;
        } else if (index.column() == 3) {
            return issue.filename;
        } else if (index.column() == 4) {
            return issue.line;
        } else if (index.column() == 5) {
            return issue.column;
        }
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
