#pragma once
#include <QWidget>
#include "process/CargoManager.h"
#include <QJsonObject>

namespace Ui {
    class ProjectProperties;
}

class ProjectProperties : public QWidget {
    Q_OBJECT

public:
    explicit ProjectProperties(QWidget* parent = nullptr);
    ~ProjectProperties();

    CargoManager::BuildTarget getBuildTarget() const;
    void setBuildTarget(CargoManager::BuildTarget buildTarget);

    const QString getRunTarget() const;
    void setProject(const QString& projectPath);

    QString getArguments() const;
    QStringList getArgumentsList() const;
    void setArguments(const QString& arguments);

    void reset();
    void updateMetadata();

private:
    Ui::ProjectProperties* ui;
    QString projectPath;
    QJsonObject metadata;
};
