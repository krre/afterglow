find_package(Qt6 REQUIRED COMPONENTS Network)

add_library(common STATIC
    core/Constants.h
    core/async/CoTask.h
    core/Constants.h
    core/FileDownloader.cpp core/FileDownloader.h
    core/Global.cpp core/Global.h
    core/Settings.cpp core/Settings.h
    core/Utils.cpp core/Utils.h
    process/CargoManager.cpp process/CargoManager.h
    process/ProcessManager.cpp process/ProcessManager.h
    process/RlsManager.cpp process/RlsManager.h
    resources/highlighting/rust.json
    resources/prefs.json
    texteditor/AutoCompleter.cpp texteditor/AutoCompleter.h
    texteditor/Highlighter.cpp texteditor/Highlighter.h
    texteditor/LineNumberArea.cpp texteditor/LineNumberArea.h
    texteditor/SyntaxHighlightManager.cpp texteditor/SyntaxHighlightManager.h
    texteditor/TextEditor.cpp texteditor/TextEditor.h
    ui/base/BrowseLayout.cpp ui/base/BrowseLayout.h
    ui/base/StandardDialog.cpp ui/base/StandardDialog.h
    ui/installer/AddComponentOrTarget.cpp ui/installer/AddComponentOrTarget.h
    ui/installer/ComponentTab.cpp ui/installer/ComponentTab.h
    ui/installer/InstallerTab.cpp ui/installer/InstallerTab.h
    ui/installer/InstallToolchain.cpp ui/installer/InstallToolchain.h
    ui/installer/OverrideTab.cpp ui/installer/OverrideTab.h
    ui/installer/RustInstaller.cpp ui/installer/RustInstaller.h
    ui/installer/RustupTab.cpp ui/installer/RustupTab.h
    ui/installer/SelectableListView.cpp ui/installer/SelectableListView.h
    ui/installer/SetOverride.cpp ui/installer/SetOverride.h
    ui/installer/TargetTab.cpp ui/installer/TargetTab.h
    ui/installer/ToolchainTab.cpp ui/installer/ToolchainTab.h
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
    ui/StringListModel.cpp ui/StringListModel.h
    core/Application.h core/Application.cpp
)

target_link_libraries(common PRIVATE Qt6::Widgets Qt6::Network)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
