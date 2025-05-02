#pragma once
#include <QChar>

namespace Const {

namespace Environment {
    constexpr auto RustupHome = "RUSTUP_HOME";
    constexpr auto CargoHome = "CARGO_HOME";
    constexpr auto Path = "PATH";
}

namespace Window {
    constexpr auto WorkspaceDir = "RustProjects";
    constexpr int MaxRecentFiles = 10;
}

namespace Project {
    constexpr auto DataDir = ".afterglow";
    constexpr auto SessionFile = "session.json";
    constexpr auto PropertyFile = "properties.json";
}

namespace FontAwesome {
    constexpr auto Cog = QChar(0xf013);
    constexpr auto Stop = QChar(0xf04d);
    constexpr auto TrashAlt = QChar(0xf2ed);
    constexpr auto Play = QChar(0xf04b);
    constexpr auto ExclamationTriangle = QChar(0xf071);
    constexpr auto TimesCircle = QChar(0xf057);
    constexpr auto Hammer = QChar(0xf6e3);
}

namespace Color {
    constexpr auto Message = "#0000ff";
    constexpr auto Line = "#e0eff5";
    constexpr auto LineNumberArea = "#f0f0f0";
    constexpr auto LineNumber = "#aaaaaa";
    constexpr auto IssueSeparator = "#969696";
    constexpr auto WarningIssueIcon = "#ffd800";
    constexpr auto ErrorIssueIcon = "#ff3600";
}

namespace Action {
    constexpr auto NewProject = "NewProject";
    constexpr auto NewRustFile = "NewRustFile";
    constexpr auto NewFile = "NewFile";
    constexpr auto NewDirectory = "NewDirectory";

    constexpr auto Open = "Open";
    constexpr auto CloseProject = "CloseProject";

    constexpr auto Save = "Save";
    constexpr auto SaveAs = "SaveAs";
    constexpr auto SaveAll = "SaveAll";

    constexpr auto Close = "Close";
    constexpr auto CloseAll = "CloseAll";
    constexpr auto CloseOther = "CloseOther";

    constexpr auto Build = "Build";
    constexpr auto Run = "Run";
    constexpr auto Check = "Check";
    constexpr auto Stop = "Stop";
    constexpr auto Clean = "Clean";
    constexpr auto Doc = "Doc";

    constexpr auto ShowSidebar = "ShowSidebar";
    constexpr auto ShowOutput = "ShowOutput";
}

}
