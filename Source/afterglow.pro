QT += core gui widgets network

CONFIG += c++17
TARGET = afterglow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    UI/MainWindow.cpp \
    Core/Global.cpp \
    UI/SelectWorkspace.cpp \
    UI/Options.cpp \
    UI/NewProject.cpp \
    TextEditor/LineNumberArea.cpp \
    TextEditor/Highlighter.cpp \
    UI/NewName.cpp \
    UI/ProjectProperties.cpp \
    UI/Rename.cpp \
    UI/FileSystemProxyModel.cpp \
    UI/ProjectTree.cpp \
    Core/Settings.cpp \
    Process/ProcessManager.cpp \
    Process/CargoManager.cpp \
    TextEditor/AutoCompleter.cpp \
    TextEditor/TextEditor.cpp \
    TextEditor/SyntaxHighlightManager.cpp \
    UI/GoToLine.cpp \
    UI/ActionManager.cpp \
    UI/RustInstaller.cpp \
    Core/FileDownloader.cpp \
    Process/RlsManager.cpp \
    UI/CommandLine.cpp \
    UI/StringListModel.cpp \
    Core/Utils.cpp \
    UI/InstallToolchain.cpp \
    UI/AddComponentOrTarget.cpp \
    UI/SetOverride.cpp \
    UI/IssuesListView.cpp \
    UI/IssueModel.cpp

HEADERS += \
    UI/MainWindow.h \
    Core/Global.h \
    UI/SelectWorkspace.h \
    UI/Options.h \
    UI/NewProject.h \
    TextEditor/LineNumberArea.h \
    TextEditor/Highlighter.h \
    UI/NewName.h \
    UI/ProjectProperties.h \
    UI/Rename.h \
    Core/Constants.h \
    UI/FileSystemProxyModel.h \
    UI/ProjectTree.h \
    Core/Settings.h \
    Core/Singleton.h \
    Process/ProcessManager.h \
    Process/CargoManager.h \
    TextEditor/AutoCompleter.h \
    TextEditor/TextEditor.h \
    TextEditor/SyntaxHighlightManager.h \
    UI/GoToLine.h \
    UI/ActionManager.h \
    UI/RustInstaller.h \
    Core/FileDownloader.h \
    Process/RlsManager.h \
    UI/CommandLine.h \
    UI/StringListModel.h \
    Core/Utils.h \
    UI/InstallToolchain.h \
    UI/AddComponentOrTarget.h \
    UI/SetOverride.h \
    UI/IssuesListView.h \
    UI/IssueModel.h

FORMS += \
    UI/MainWindow.ui \
    UI/SelectWorkspace.ui \
    UI/Options.ui \
    UI/NewProject.ui \
    UI/NewName.ui \
    UI/ProjectProperties.ui \
    UI/Rename.ui \
    UI/GoToLine.ui \
    UI/RustInstaller.ui \
    UI/InstallToolchain.ui \
    UI/AddComponentOrTarget.ui \
    UI/SetOverride.ui

DISTFILES += \
    ../README.md \
    ../.gitignore \

RESOURCES += \
    Resources.qrc
