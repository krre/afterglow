#pragma once
#include <QWidget>
#include "Cargo/CargoManager.h"
#include <QJsonObject>

namespace Ui {
    class ProjectProperties;
}

class ProjectProperties : public QWidget {
    Q_OBJECT

public:
    explicit ProjectProperties(QWidget* parent = 0);
    ~ProjectProperties();

    CargoManager::BuildTarget getTarget() const;
    void setTarget(CargoManager::BuildTarget target);
    void setProject(const QString& projectPath);

    const QString getTargetDirectory() const;

    void reset();

private:
    Ui::ProjectProperties* ui;
    QString projectPath;
    QJsonObject metadata;
};
