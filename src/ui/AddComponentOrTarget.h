#pragma once
#include <QDialog>
#include <QStringList>

namespace Ui {
    class AddComponentOrTarget;
}

class QMenu;

class AddComponentOrTarget : public QDialog {
    Q_OBJECT

public:
    explicit AddComponentOrTarget(const QString& title, const QString& command, QWidget* parent = nullptr);
    ~AddComponentOrTarget();
    const QStringList& getList() const { return list; }

private slots:
    void on_buttonBox_accepted();
    void onCopyAction();
    void onCustomContextMenu(const QPoint& point);

private:
    Ui::AddComponentOrTarget* ui = nullptr;
    QStringList list;
    QMenu* contextMenu = nullptr;
};
