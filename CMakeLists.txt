cmake_minimum_required(VERSION 3.16)

project(q565viewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Test)

qt_standard_project_setup(REQUIRES 6.5)
set(TARGET_LINK_LIBS )
# Add CXX-Qt dependency to directly link to the q565 Rust crate
if(BUILD_RUST)
    find_package(CxxQt QUIET)
    if(NOT CxxQt_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            CxxQt
            GIT_REPOSITORY https://github.com/kdab/cxx-qt-cmake.git
            GIT_TAG 0.7.2
        )
        FetchContent_MakeAvailable(CxxQt)
    endif()
    add_custom_target(Qt_Rust_q565_Binding ALL
        SOURCES
        rust/qt_rust_q565/src/qt_rust_q565.rs
        rust/qt_rust_q565/src/lib.rs
        rust/qt_rust_q565/build.rs
        rust/qt_rust_q565/Cargo.toml
        rust/qt_rust_q565/qml/main.qml
    )
    cxx_qt_import_crate(
        MANIFEST_PATH rust/qt_rust_q565/Cargo.toml
        CRATES qt_rust_q565
        QT_MODULES Qt::Core Qt::Gui Qt::Qml Qt::QuickControls2
    )
    cxx_qt_import_qml_module(qt_rust_q565_qml_module
        URI "com.pr.rust.q565"
        SOURCE_CRATE qt_rust_q565)
    set(TARGET_LINK_LIBS qt_rust_q565_qml_module)
endif()

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
    PRIVATE Qt6::Quick Qt6::Test ${TARGET_LINK_LIBS}
)

include(GNUInstallDirs)
install(TARGETS appq565viewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
