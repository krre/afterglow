#pragma once
#include <QDialog>
#include <QStringList>

namespace Ui {
    class AddTarget;
}

class AddTarget : public QDialog {
    Q_OBJECT

public:
    explicit AddTarget(QWidget* parent = 0);
    ~AddTarget();
    const QStringList& getTargets() const { return targets; }

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddTarget* ui;
    QStringList targets;
};
