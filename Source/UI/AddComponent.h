#pragma once
#include <QDialog>
#include <QStringList>

namespace Ui {
    class AddComponent;
}

class AddComponent : public QDialog {
    Q_OBJECT

public:
    explicit AddComponent(QWidget* parent = 0);
    ~AddComponent();
    const QStringList& getComponents() const { return components; }

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddComponent* ui;
    QStringList components;
};
