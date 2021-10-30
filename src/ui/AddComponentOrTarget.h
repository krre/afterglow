#pragma once
#include "base/Dialog.h"
#include <QStringList>

class QMenu;
class QListView;

class AddComponentOrTarget : public Dialog {
    Q_OBJECT
public:
    explicit AddComponentOrTarget(const QString& title, const QString& command, QWidget* parent = nullptr);
    const QStringList& list() const { return m_list; }

protected slots:
    void accept() override;

private slots:
    void onCopyAction();
    void onCustomContextMenu(const QPoint& point);

private:
    QStringList m_list;
    QListView* listView = nullptr;
    QMenu* contextMenu = nullptr;
};
