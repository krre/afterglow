#pragma once

namespace Const {
    namespace Environment {
        const char RustupHome[] = "RUSTUP_HOME";
        const char CargoHome[] = "CARGO_HOME";
        const char Path[] = "PATH";
    }

    namespace App {
        const char Name[] = "Afterglow";
        const char Version[] = "0.2.0";
        const char Status[] = "Preview Alpha";
        const char Url[] = "https://github.com/krre/afterglow";
        const char SettingsName[] = "afterglow.ini";
        const char PrefsName[] = "prefs.json";
        const char Copyright[] = "Copyright © 2018-2020, Vladimir Zarypov";
    }

    namespace Window {
        const char WorkspaceDir[] = "RustProjects";
        const int MaxRecentFiles = 10;
        const int SeparatorAndMenuClearCount = 2;
    }

    namespace Project {
        const char DataDir[] = ".afterglow";
        const char SessionFile[] = "session.json";
        const char PropertyFile[] = "properties.json";
    }

    namespace FontAwesome {
        const char Cog[] = "\uf013";
        const char Stop[] = "\uf04d";
        const char TrashAlt[] = "\uf2ed";
        const char Play[] = "\uf04b";
        const char ExclamationTriangle[] = "\uf071";
        const char TimesCircle[] = "\uf057";
    }

    namespace Color {
        const char Message[] = "#0000ff";
        const char Line[] = "#e0eff5";
        const char LineNumberArea[] = "#f0f0f0";
        const char LineNumber[] = "#aaaaaa";
        const char IssueSeparator[] = "#969696";
        const char WarningIssueIcon[] = "#ffd800";
        const char ErrorIssueIcon[] = "#ff3600";

    }

    namespace Action {
        const char NewProject[] = "NewProject";
        const char NewRustFile[] = "NewRustFile";
        const char NewFile[] = "NewFile";
        const char NewDirectory[] = "NewDirectory";

        const char Open[] = "Open";
        const char CloseProject[] = "CloseProject";

        const char Save[] = "Save";
        const char SaveAs[] = "SaveAs";
        const char SaveAll[] = "SaveAll";

        const char Close[] = "Close";
        const char CloseAll[] = "CloseAll";
        const char CloseOther[] = "CloseOther";

        const char Build[] = "Build";
        const char Run[] = "Run";
        const char Stop[] = "Stop";
        const char Clean[] = "Clean";
    }
}
