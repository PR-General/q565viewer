cmake_minimum_required(VERSION 3.16)

project(q565viewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Test)

qt_standard_project_setup(REQUIRES 6.5)

set(DEPLOY_Q565_PLUGIN_TO ${CMAKE_BINARY_DIR};${CMAKE_BINARY_DIR}/tests)

set(QT_IMAGE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/images/)


# Include the Q565 image format plugin
add_subdirectory(q565-io-plugin)

# enable and include (all) unit tests
enable_testing(true)
add_subdirectory(tests/)

qt_add_executable(appq565viewer
    main.cpp
)



qt_add_qml_module(appq565viewer
    URI q565viewer
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES q565transcoder.h  q565transcoder.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appq565viewer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appq565viewer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appq565viewer
    PRIVATE Qt6::Quick Qt6::Test
)

include(GNUInstallDirs)
install(TARGETS appq565viewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
