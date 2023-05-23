#pragma once
#include <QListView>

class InstallerListView : public QListView {
public:
    InstallerListView();
    QStringList selectedRows() const;

private slots:
    void onCustomContextMenu(const QPoint& point);
    void onCopyAction();
};
