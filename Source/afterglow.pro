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
    UI/NewProject.cpp \
    Editor/LineNumberArea.cpp \
    Editor/Highlighter.cpp \
    UI/NewName.cpp \
    UI/ProjectProperties.cpp \
    UI/Rename.cpp \
    Editor/Editor.cpp \
    UI/FileSystemProxyModel.cpp \
    UI/ProjectTree.cpp \
    Core/Settings.cpp \
    Core/ProcessManager.cpp

HEADERS += \
    UI/MainWindow.h \
    Core/Global.h \
    UI/SelectWorkspace.h \
    UI/Options.h \
    Cargo/CargoManager.h \
    Editor/Editor.h \
    UI/NewProject.h \
    Editor/LineNumberArea.h \
    Editor/Highlighter.h \
    UI/NewName.h \
    UI/ProjectProperties.h \
    UI/Rename.h \
    Core/Constants.h \
    UI/FileSystemProxyModel.h \
    UI/ProjectTree.h \
    Core/Settings.h \
    Core/Singleton.h \
    Core/ProcessManager.h

FORMS += \
    UI/MainWindow.ui \
    UI/SelectWorkspace.ui \
    UI/Options.ui \
    UI/NewProject.ui \
    UI/NewName.ui \
    UI/ProjectProperties.ui \
    UI/Rename.ui

DISTFILES += \
    ../README.md \
    ../.gitignore \

RESOURCES += \
    Resources.qrc
