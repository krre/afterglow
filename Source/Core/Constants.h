#pragma once

namespace Constants {
    namespace App {
        const char NAME[] = "Afterglow";
        const char VERSION[] = "0.2.0";
        const char STATUS[] = "Preview Alpha";
        const char URL[] = "https://github.com/krre/afterglow";
        const char SETTINGS_NAME[] = "afterglow.ini";
        const char PREFS_NAME[] = "prefs.json";
        const char COPYRIGHT[] = "Copyright © 2018, Vladimir Zarypov";
    }

    namespace Window {
        const char WORKSPACE_DIRECTORY[] = "RustProjects";
        const int MAX_RECENT_FILES = 10;
        const int SEPARATOR_AND_MENU_CLEAR_COUNT = 2;
    }

    namespace Project {
        const char DATA_DIRECTORY[] = ".afterglow";
        const char SESSION_FILE[] = "session.json";
        const char PROPERTIES_FILE[] = "properties.json";
    }

    namespace FontAwesome {
        const char COG[] = "\uf013";
        const char STOP[] = "\uf04d";
        const char TRASH_ALT[] = "\uf2ed";
        const char PLAY[] = "\uf04b";
    }

    namespace Action {
        const char NEW_PROJECT[] = "NewProject";
        const char NEW_RUST_FILE[] = "NewRustFile";
        const char NEW_FILE[] = "NewFile";
        const char NEW_DIRECTORY[] = "NewDirectory";

        const char OPEN[] = "Open";
        const char CLOSE_PROJECT[] = "CloseProject";

        const char SAVE[] = "Save";
        const char SAVE_AS[] = "SaveAs";
        const char SAVE_ALL[] = "SaveAll";

        const char CLOSE[] = "Close";
        const char CLOSE_ALL[] = "CloseAll";
        const char CLOSE_OTHER[] = "CloseOther";
    }
}
