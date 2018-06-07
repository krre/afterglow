#pragma once
#include <QObject>
#include <QString>
#include <QFont>

class Global : public QObject {
    Q_OBJECT

public:
    Global(QObject* parent = nullptr);
    static void init();

    static QString getWorkspacePath();
    static QString getSystemRustupHome();
    static QString getSystemCargoHome();
    static QFont getFontAwesomeFont();
};
