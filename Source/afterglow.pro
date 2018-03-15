QT += core gui widgets

CONFIG += c++11
TARGET = afterglow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    UI/MainWindow.cpp \
    Core/Global.cpp \
    UI/SelectWorkspace.cpp \
    UI/Options.cpp \
    Cargo/CargoManager.cpp \
    UI/ProjectTreeView.cpp \
    Editor/Editor.cpp \
    UI/NewFile.cpp \
    UI/NewProject.cpp \
    Editor/LineNumberArea.cpp \
    Editor/Highlighter.cpp

HEADERS += \
    UI/MainWindow.h \
    Core/Global.h \
    Core/Constants.h \
    UI/SelectWorkspace.h \
    UI/Options.h \
    Cargo/CargoManager.h \
    UI/ProjectTreeView.h \
    Editor/Editor.h \
    UI/NewFile.h \
    UI/NewProject.h \
    Editor/LineNumberArea.h \
    Editor/Highlighter.h

FORMS += \
    UI/MainWindow.ui \
    UI/SelectWorkspace.ui \
    UI/Options.ui \
    UI/NewFile.ui \
    UI/NewProject.ui

DISTFILES += \
    ../README.md \
    ../.gitignore
