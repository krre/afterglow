#pragma once
#include <QDialog>
#include <QStringList>

namespace Ui {
    class AddComponentOrTarget;
}

class AddComponentOrTarget : public QDialog {
    Q_OBJECT

public:
    explicit AddComponentOrTarget(const QString& title, const QString& command, QWidget* parent = 0);
    ~AddComponentOrTarget();
    const QStringList& getList() const { return list; }

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddComponentOrTarget* ui;
    QStringList list;
};
