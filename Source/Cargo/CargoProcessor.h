#pragma once
#include <QObject>

class CargoProcessor : public QObject {
    Q_OBJECT
public:
    explicit CargoProcessor(QObject* parent = nullptr);
    ~CargoProcessor();
    void createProject(const QString& command);

signals:

public slots:
};
