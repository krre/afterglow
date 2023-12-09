set(SOURCES main.cpp)
qt_add_resources(SOURCES ../src/resources.qrc)
add_executable(afterglow ${SOURCES})

target_link_libraries(afterglow PRIVATE
    Qt6::Widgets
    common
)
