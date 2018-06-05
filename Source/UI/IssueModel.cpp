#include "IssueModel.h"
#include <QtCore>

IssueModel::IssueModel(QObject* parent) : QAbstractItemModel(parent) {
}

IssueModel::~IssueModel() {
}

void IssueModel::appendMessage(const QJsonObject& message) {
//    qDebug() << message;
    QJsonObject messageObj = message["message"].toObject();
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

    QJsonObject spanObj = messageObj["spans"].toArray().at(0).toObject();
    issue.filename = spanObj["file_name"].toString();
    issue.line = spanObj["line_start"].toInt();
    issue.column = spanObj["column_start"].toInt();

    beginInsertRows(QModelIndex(), issues.count(), issues.count());
    qDebug() << "append";
    issues.append(issue);
    endInsertRows();
}

void IssueModel::clear() {
    if (!issues.count()) return;

    beginRemoveRows(QModelIndex(), 0, qMax(issues.count() - 1, 0));
    issues.clear();
    endRemoveRows();
}

int IssueModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return ROLES_COUNT;
}

QVariant IssueModel::data(const QModelIndex& index, int role) const {
    qDebug() << "IssueModel::data" << index << role << issues.count();
    if (!index.isValid()) return QVariant();

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags IssueModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return 0;

    return QAbstractItemModel::flags(index);
}

QModelIndex IssueModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    return QModelIndex();
}

QModelIndex IssueModel::parent(const QModelIndex& index) const {
    if (!index.isValid()) return QModelIndex();

    return QModelIndex();
}

int IssueModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    qDebug() << "issueModel::rowCount" << parent << issues.count();
    return issues.count();
}
