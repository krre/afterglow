#pragma once
#include <QObject>
#include <QString>

class Global : public QObject {
    Q_OBJECT

public:
    Global(QObject* parent = nullptr);

    static QString getPortableSettingsPath();
    static QString getDefaultWorkspacePath();
};
