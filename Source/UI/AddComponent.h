#pragma once
#include <QDialog>

namespace Ui {
    class AddComponent;
}

class AddComponent : public QDialog {
    Q_OBJECT

public:
    explicit AddComponent(QWidget* parent = 0);
    ~AddComponent();

private:
    Ui::AddComponent* ui;
};
