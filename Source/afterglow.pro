QT += core gui widgets

CONFIG += c++11
TARGET = afterglow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    UI/MainWindow.cpp

HEADERS += \
    UI/MainWindow.h \
    Constants.h

FORMS += \
    UI/MainWindow.ui

DISTFILES += \
    ../README.md \
    ../.gitignore
