cmake_minimum_required(VERSION 3.22)

project(afterglow VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(tests)

