#pragma once
#include <QString>
#include <QFont>

class Global {
public:
    static void init();

    static QString workspacePath();
    static QString systemRustupHome();
    static QString systemCargoHome();
    static QFont fontAwesomeFont();
};
