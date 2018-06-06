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

    int count = issues.count();
    insertRows(count, 1, QModelIndex());
    QModelIndex idx = index(count, 0, QModelIndex());
    setData(idx, static_cast<int>(issue.level), Qt::EditRole);
    idx = index(count, 1, QModelIndex());
    setData(idx, issue.message, Qt::EditRole);
}

void IssueModel::clear() {
    if (!issues.count()) return;

    removeRows(0, issues.count(), QModelIndex());
}

int IssueModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return ROLES_COUNT;
}

bool IssueModel::insertRows(int row, int count, const QModelIndex& parent) {
    Q_UNUSED(parent)

    beginInsertRows(QModelIndex(), issues.count(), issues.count());
    qDebug() << "IssueModel::insertRows" << row << count;
    for (int i = 0; i < count; i++) {
        Issue issue;
        issues.insert(row, issue);
    }

    endInsertRows();

    return true;
}

bool IssueModel::removeRows(int row, int count, const QModelIndex& parent) {
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; ++i)
        issues.removeAt(i);

    endRemoveRows();

    return true;
}

QVariant IssueModel::data(const QModelIndex& index, int role) const {
    qDebug() << "IssueModel::data" << index << role << issues.count();
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

bool IssueModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        Issue issue = issues.value(row);

        if (index.column() == 0) {
            issue.level = static_cast<Issue::Level>(value.toInt());
        } else if (index.column() == 1) {
           issue.message = value.toString();
        } else if (index.column() == 2) {
            issue.rendered = value.toString();
        } else if (index.column() == 3) {
            issue.filename = value.toString();
        } else if (index.column() == 4) {
            issue.line = value.toInt();
        } else if (index.column() == 5) {
            issue.column = value.toInt();
        } else {
            return false;
        }

        issues.replace(row, issue);
        emit(dataChanged(index, index));

        return true;
    }

    return false;
}

Qt::ItemFlags IssueModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QModelIndex IssueModel::index(int row, int column, const QModelIndex& parent) const {
    if (parent.isValid()) return QModelIndex();
    return createIndex(row, column);
}

QModelIndex IssueModel::parent(const QModelIndex& index) const {
    Q_UNUSED(index)
    return QModelIndex();
}

int IssueModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    qDebug() << "issueModel::rowCount" << parent << issues.count();
    return issues.count();
}
