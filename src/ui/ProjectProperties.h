#pragma once
#include "process/CargoManager.h"
#include <QWidget>
#include <QJsonObject>

class QComboBox;
class QLineEdit;

class ProjectProperties : public QWidget {
    Q_OBJECT
public:
    explicit ProjectProperties(QWidget* parent = nullptr);

    CargoManager::BuildTarget buildTarget() const;
    void setBuildTarget(CargoManager::BuildTarget buildTarget);

    const QString runTarget() const;
    void setProject(const QString& projectPath);

    QString arguments() const;
    QStringList argumentsList() const;
    void setArguments(const QString& arguments);

    void reset();
    void updateMetadata();

private:
    QComboBox* targetComboBox = nullptr;
    QComboBox* runComboBox = nullptr;
    QLineEdit* argumentsLineEdit = nullptr;
    QString projectPath;
    QJsonObject metadata;
};
