QT += core gui widgets network

CONFIG += c++20
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
    ui/base/StandardDialog.cpp \
    ui/installer/AddComponentOrTarget.cpp \
    ui/installer/ComponentTab.cpp \
    ui/installer/InstallToolchain.cpp \
    ui/installer/InstallerTab.cpp \
    ui/installer/OverrideTab.cpp \
    ui/installer/RustInstaller.cpp \
    ui/installer/RustupTab.cpp \
    ui/installer/SelectableListView.cpp \
    ui/installer/SetOverride.cpp \
    ui/installer/TargetTab.cpp \
    ui/installer/ToolchainTab.cpp

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
    ui/base/StandardDialog.h \
    ui/installer/AddComponentOrTarget.h \
    ui/installer/ComponentTab.h \
    ui/installer/InstallToolchain.h \
    ui/installer/InstallerTab.h \
    ui/installer/OverrideTab.h \
    ui/installer/RustInstaller.h \
    ui/installer/RustupTab.h \
    ui/installer/SelectableListView.h \
    ui/installer/SetOverride.h \
    ui/installer/TargetTab.h \
    ui/installer/ToolchainTab.h

FORMS +=

DISTFILES += \
    ../README.md \
    ../.gitignore \

RESOURCES += \
    resources.qrc
