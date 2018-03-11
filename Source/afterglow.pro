QT += core gui widgets

CONFIG += c++11
TARGET = afterglow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    UI/MainWindow.cpp \
    Core/Global.cpp \
    UI/NewCargoProject.cpp \
    UI/SelectWorkspace.cpp

HEADERS += \
    UI/MainWindow.h \
    Core/Global.h \
    Core/Constants.h \
    UI/NewCargoProject.h \
    UI/SelectWorkspace.h

FORMS += \
    UI/MainWindow.ui \
    UI/NewCargoProject.ui \
    UI/SelectWorkspace.ui

DISTFILES += \
    ../README.md \
    ../.gitignore
