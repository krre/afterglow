#pragma once
#include <QObject>
#include <QString>

class Global : public QObject {
    Q_OBJECT

public:
    Global(QObject* parent = nullptr);
    static void init();

    static QString getWorkspacePath();
    static QString getSystemRustupHome();
    static QString getSystemCargoHome();
};
