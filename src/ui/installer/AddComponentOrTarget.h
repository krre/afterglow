#pragma once
#include "ui/base/StandardDialog.h"
#include <QStringList>

class SelectableListView;

class AddComponentOrTarget : public StandardDialog {
    Q_OBJECT
public:
    explicit AddComponentOrTarget(const QString& title, const QString& command, QWidget* parent = nullptr);
    const QStringList& list() const { return m_list; }

protected slots:
    void accept() override;

private:
    QStringList m_list;
    SelectableListView* listView = nullptr;
};
