#include "Application.h"

Application::Application(int& argc, char* argv[]) : QApplication(argc, argv) {
    setOrganizationName(Organization);
    setApplicationName(Name);
    setApplicationVersion(Version);
}
