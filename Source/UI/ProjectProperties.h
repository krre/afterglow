#pragma once
#include <QWidget>

namespace Ui {
    class ProjectProperties;
}

class ProjectProperties : public QWidget {
    Q_OBJECT

public:
    explicit ProjectProperties(QWidget* parent = 0);
    ~ProjectProperties();

private:
    Ui::ProjectProperties* ui;
};
