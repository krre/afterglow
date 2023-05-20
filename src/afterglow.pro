QT += core gui widgets network

CONFIG += c++17
TARGET = afterglow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    ui/MainWindow.cpp \
    core/Global.cpp \
    ui/Preferences.cpp \
    ui/SelectWorkspace.cpp \
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
    core/FileDownloader.cpp \
    process/RlsManager.cpp \
    ui/CommandLine.cpp \
    ui/StringListModel.cpp \
    core/Utils.cpp \
    ui/IssueModel.cpp \
    ui/IssueListView.cpp \
    ui/base/BrowseLayout.cpp \
    ui/base/Dialog.cpp \
    ui/installer/AddComponentOrTarget.cpp \
    ui/installer/InstallToolchain.cpp \
    ui/installer/RustInstaller.cpp \
    ui/installer/SetOverride.cpp

HEADERS += \
    core/Constants.h \
    ui/MainWindow.h \
    core/Global.h \
    ui/Preferences.h \
    ui/SelectWorkspace.h \
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
    core/FileDownloader.h \
    process/RlsManager.h \
    ui/CommandLine.h \
    ui/StringListModel.h \
    core/Utils.h \
    ui/IssueModel.h \
    ui/IssueListView.h \
    ui/base/BrowseLayout.h \
    ui/base/Dialog.h \
    ui/installer/AddComponentOrTarget.h \
    ui/installer/InstallToolchain.h \
    ui/installer/RustInstaller.h \
    ui/installer/SetOverride.h

FORMS +=

DISTFILES += \
    ../README.md \
    ../.gitignore \

RESOURCES += \
    resources.qrc
