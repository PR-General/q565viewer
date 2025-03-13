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
foreach(DEPLOY_PATH ${DEPLOY_Q565_PLUGIN_TO})
        message("Deploying plugin to: " ${DEPLOY_PATH}/imageformats/)
        file(MAKE_DIRECTORY ${DEPLOY_PATH}/imageformats/)
        add_custom_command(TARGET q565
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy libq565.so ${DEPLOY_PATH}/imageformats/
        )
endforeach()
