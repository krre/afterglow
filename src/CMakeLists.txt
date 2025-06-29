configure_file(config.h.in config.h @ONLY)

add_library(common STATIC
    core/Application.h core/Application.cpp
    core/Constants.h
    core/Constants.h
    core/FileDownloader.cpp core/FileDownloader.h
    core/Global.cpp core/Global.h
    core/Settings.cpp core/Settings.h
    core/Utils.cpp core/Utils.h
    core/async/CoTask.h
    process/CargoManager.cpp process/CargoManager.h
    process/ProcessManager.cpp process/ProcessManager.h
    process/RlsManager.cpp process/RlsManager.h
    ui/ActionManager.cpp ui/ActionManager.h
    ui/CommandLine.cpp ui/CommandLine.h
    ui/FileSystemProxyModel.cpp ui/FileSystemProxyModel.h
    ui/GoToLine.cpp ui/GoToLine.h
    ui/IssueListView.cpp ui/IssueListView.h
    ui/IssueModel.cpp ui/IssueModel.h
    ui/MainWindow.cpp ui/MainWindow.h
    ui/NewName.cpp ui/NewName.h
    ui/NewProject.cpp ui/NewProject.h
    ui/Preferences.cpp ui/Preferences.h
    ui/ProjectProperties.cpp ui/ProjectProperties.h
    ui/ProjectTree.cpp ui/ProjectTree.h
    ui/Rename.cpp ui/Rename.h
    ui/SelectWorkspace.cpp ui/SelectWorkspace.h
    ui/SourceTabWidget.h ui/SourceTabWidget.cpp
    ui/StringListModel.cpp ui/StringListModel.h
    ui/installer/AddComponentOrTarget.cpp ui/installer/AddComponentOrTarget.h
    ui/installer/ComponentTab.cpp ui/installer/ComponentTab.h
    ui/installer/InstallToolchain.cpp ui/installer/InstallToolchain.h
    ui/installer/InstallerTab.cpp ui/installer/InstallerTab.h
    ui/installer/OverrideTab.cpp ui/installer/OverrideTab.h
    ui/installer/RustInstaller.cpp ui/installer/RustInstaller.h
    ui/installer/RustupTab.cpp ui/installer/RustupTab.h
    ui/installer/SelectableListView.cpp ui/installer/SelectableListView.h
    ui/installer/SetOverride.cpp ui/installer/SetOverride.h
    ui/installer/TargetTab.cpp ui/installer/TargetTab.h
    ui/installer/ToolchainTab.cpp ui/installer/ToolchainTab.h
    ui/texteditor/AutoCompleter.cpp ui/texteditor/AutoCompleter.h
    ui/texteditor/Highlighter.cpp ui/texteditor/Highlighter.h
    ui/texteditor/LineNumberArea.cpp ui/texteditor/LineNumberArea.h
    ui/texteditor/SyntaxHighlightManager.cpp ui/texteditor/SyntaxHighlightManager.h
    ui/texteditor/TextEditor.cpp ui/texteditor/TextEditor.h
    ui/widgets/BrowseLayout.cpp ui/widgets/BrowseLayout.h
    ui/widgets/IconToolButton.h ui/widgets/IconToolButton.cpp
    ui/widgets/RecentMenu.h ui/widgets/RecentMenu.cpp
    ui/widgets/StandardDialog.cpp ui/widgets/StandardDialog.h
)

# For visibility in IDE (e.g., Qt Creator)
add_custom_target(custom
    SOURCES
    config.h.in
)

target_link_libraries(common PUBLIC Qt6::Widgets Qt6::Network)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
