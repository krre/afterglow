#pragma once
#include <QListView>
#include <QStyledItemDelegate>

class IssueModel;

class IssueDelegate : public QStyledItemDelegate {
public:
    IssueDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void currentChanged(const QModelIndex& current, const QModelIndex& previous);
};

class IssueListView : public QListView {
public:
    explicit IssueListView(IssueModel* model, QWidget* parent = nullptr);

private slots:
    void onCustomContextMenu(const QPoint& point);
    void onCopyLabelAction();
    void onCopyDescriptionAction();

private:
    void copyRowToClipboard(int role);

    QMenu* m_contextMenu = nullptr;
};
