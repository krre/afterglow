#pragma once
#include <QWidget>
#include "Process/CargoManager.h"
#include <QJsonObject>

namespace Ui {
    class ProjectProperties;
}

class ProjectProperties : public QWidget {
    Q_OBJECT

public:
    explicit ProjectProperties(QWidget* parent = 0);
    ~ProjectProperties();

    CargoManager::BuildTarget getBuildTarget() const;
    void setBuildTarget(CargoManager::BuildTarget buildTarget);

    const QString getRunTarget() const;
    void setProject(const QString& projectPath);

    void reset();
    void updateMetadata();

private:
    Ui::ProjectProperties* ui;
    QString projectPath;
    QJsonObject metadata;
};
