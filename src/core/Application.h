#pragma once
#include "config.h"
#include <QApplication>

class Application : public QApplication {
public:
    static constexpr auto Organization = "Afterglow";
    static constexpr auto Name = "Afterglow";
    static constexpr auto Version = PROJECT_VERSION;
    static constexpr auto Status = "Preview Alpha";
    static constexpr auto Url = "https://github.com/krre/afterglow";
    static constexpr auto PrefsName = "prefs.json";
    static constexpr auto Copyright = "Copyright © 2018-2024, Vladimir Zarypov";
    static constexpr auto BuildDate = __DATE__;
    static constexpr auto BuildTime = __TIME__;

    Application(int& argc, char* argv[]);
};
