#pragma once

namespace Const {
    namespace Environment {
        constexpr auto RustupHome = "RUSTUP_HOME";
        constexpr auto CargoHome = "CARGO_HOME";
        constexpr auto Path = "PATH";
    }

    namespace App {
        constexpr auto Name = "Afterglow";
        constexpr auto Version = "0.2.0";
        constexpr auto Status = "Preview Alpha";
        constexpr auto Url = "https://github.com/krre/afterglow";
        constexpr auto SettingsName = "afterglow.ini";
        constexpr auto PrefsName = "prefs.json";
        constexpr auto Copyright = "Copyright © 2018-2021, Vladimir Zarypov";
    }

    namespace Window {
        constexpr auto WorkspaceDir = "RustProjects";
        constexpr int MaxRecentFiles = 10;
        constexpr int SeparatorAndMenuClearCount = 2;
    }

    namespace Project {
        constexpr auto DataDir = ".afterglow";
        constexpr auto SessionFile = "session.json";
        constexpr auto PropertyFile = "properties.json";
    }

    namespace FontAwesome {
        constexpr auto Cog = "\uf013";
        constexpr auto Stop = "\uf04d";
        constexpr auto TrashAlt = "\uf2ed";
        constexpr auto Play = "\uf04b";
        constexpr auto ExclamationTriangle = "\uf071";
        constexpr auto TimesCircle = "\uf057";
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
        constexpr auto Stop = "Stop";
        constexpr auto Clean = "Clean";

        constexpr auto ShowSidebar = "ShowSidebar";
        constexpr auto ShowOutput = "ShowOutput";
    }
}
