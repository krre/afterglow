QT += core gui widgets network

CONFIG += c++17
TARGET = afterglow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    ui/MainWindow.cpp \
    core/Global.cpp \
    ui/SelectWorkspace.cpp \
    ui/Options.cpp \
    ui/NewProject.cpp \
    texteditor/LineNumberArea.cpp \
    texteditor/Highlighter.cpp \
    ui/NewName.cpp \
    ui/ProjectProperties.cpp \
    ui/Rename.cpp \
    ui/FileSystemProxyModel.cpp \
    ui/ProjectTree.cpp \
    core/Settings.cpp \
    process/ProcessManager.cpp \
    process/CargoManager.cpp \
    texteditor/AutoCompleter.cpp \
    texteditor/TextEditor.cpp \
    texteditor/SyntaxHighlightManager.cpp \
    ui/GoToLine.cpp \
    ui/ActionManager.cpp \
    ui/RustInstaller.cpp \
    core/FileDownloader.cpp \
    process/RlsManager.cpp \
    ui/CommandLine.cpp \
    ui/StringListModel.cpp \
    core/Utils.cpp \
    ui/InstallToolchain.cpp \
    ui/AddComponentOrTarget.cpp \
    ui/SetOverride.cpp \
    ui/IssueModel.cpp \
    ui/IssueListView.cpp

HEADERS += \
    core/Const.h \
    ui/MainWindow.h \
    core/Global.h \
    ui/SelectWorkspace.h \
    ui/Options.h \
    ui/NewProject.h \
    texteditor/LineNumberArea.h \
    texteditor/Highlighter.h \
    ui/NewName.h \
    ui/ProjectProperties.h \
    ui/Rename.h \
    ui/FileSystemProxyModel.h \
    ui/ProjectTree.h \
    core/Settings.h \
    core/Singleton.h \
    process/ProcessManager.h \
    process/CargoManager.h \
    texteditor/AutoCompleter.h \
    texteditor/TextEditor.h \
    texteditor/SyntaxHighlightManager.h \
    ui/GoToLine.h \
    ui/ActionManager.h \
    ui/RustInstaller.h \
    core/FileDownloader.h \
    process/RlsManager.h \
    ui/CommandLine.h \
    ui/StringListModel.h \
    core/Utils.h \
    ui/InstallToolchain.h \
    ui/AddComponentOrTarget.h \
    ui/SetOverride.h \
    ui/IssueModel.h \
    ui/IssueListView.h

FORMS += \
    ui/MainWindow.ui \
    ui/SelectWorkspace.ui \
    ui/Options.ui \
    ui/NewProject.ui \
    ui/NewName.ui \
    ui/ProjectProperties.ui \
    ui/RustInstaller.ui \
    ui/InstallToolchain.ui \
    ui/AddComponentOrTarget.ui \
    ui/SetOverride.ui

DISTFILES += \
    ../README.md \
    ../.gitignore \

RESOURCES += \
    resources.qrc
