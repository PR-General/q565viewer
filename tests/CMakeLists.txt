cmake_minimum_required(VERSION 3.16)
project(q565benchmark LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Test)

macro(Q565Test TestName)
    set(targetName "${TestName}")
    set(testProjectName "${TestName}")
    string(TOLOWER ${TestName} TestSourceFileName)
    add_executable(${targetName}
        q565tests.h
        "${TestSourceFileName}.cpp")
    target_include_directories(${targetName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(${targetName} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Test)
    if(DEFINED QT_IMAGE_ROOT_DIR)
        target_compile_definitions(${targetName} PUBLIC QT_IMAGE_ROOT_DIR="${QT_IMAGE_ROOT_DIR}")
    endif()
    set_target_properties(${targetName} PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
    add_test(NAME ${testProjectName} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" COMMAND $<TARGET_FILE:${targetName}>)
endmacro()

Q565Test(q565benchmarking)

target_sources(q565benchmarking
  PRIVATE
    CorruptImageViewer.qml
)

target_compile_definitions(q565benchmarking PUBLIC Q565_TEST_QML_MAIN="${CMAKE_CURRENT_SOURCE_DIR}/CorruptImageViewer.qml")
