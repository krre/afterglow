#pragma once
#include <QListView>

class SelectableListView : public QListView {
public:
    SelectableListView(const QStringList& rows = QStringList());
    QStringList selectedRows() const;
    QString findDefault() const;
    void load(const QString& command, const std::function<void(QStringList&)>& filter = nullptr);

private slots:
    void onCustomContextMenu(const QPoint& point);
    void onCopyAction();
};
