#pragma once
#include <QListView>

class SelectableListView : public QListView {
public:
    SelectableListView();
    QStringList selectedRows() const;

private slots:
    void onCustomContextMenu(const QPoint& point);
    void onCopyAction();
};
