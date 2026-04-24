cmake_minimum_required(VERSION 4.3)

project(afterglow VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(REQUIRED_QT_VERSION 6.11.0)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Widgets
    Network
)

qt_standard_project_setup(
    REQUIRES ${REQUIRED_QT_VERSION}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tests)

# For visibility in IDE (e.g., Qt Creator)
add_custom_target(other
    SOURCES
    README.md
)
