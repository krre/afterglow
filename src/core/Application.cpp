#include "Application.h"
#include "core/Constants.h"

Application::Application(int& argc, char* argv[]) : QApplication(argc, argv) {
    setOrganizationName(Const::App::Organization);
    setApplicationName(Const::App::Name);
    setApplicationVersion(Const::App::Version);
}
