set(SOURCES main.cpp)
qt_add_resources(SOURCES ../resources.qrc)
qt_add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    common
)
