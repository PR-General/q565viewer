cmake_minimum_required(VERSION 3.16)
project(Q565ImageIOPlugin)

find_package(Qt6 COMPONENTS Core Gui REQUIRED)

qt_add_plugin(q565 SHARED q565.h q565imageformatplugin.cpp q565imageformatplugin.hpp)

target_sources(q565
  PRIVATE
    q565imageiohandler.hpp
    q565imageiohandler.cpp
)

# Link against Qt libraries
target_link_libraries(q565
    PRIVATE
    Qt::Gui
)

target_include_directories(q565 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# When the library is built, copy it into the path of the applications build directy if the plugin directory was defined
if(QT_DEPLOY_PLUGINS_DIR)
    add_custom_command(TARGET q565
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy libq565.so ${QT_DEPLOY_PLUGINS_DIR}/imageformats
    )
endif()
