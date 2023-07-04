cmake_minimum_required(VERSION 3.22.0)
project(BluetoothDemo VERSION 0.1.0)

cmake_policy(SET CMP0074 NEW)
enable_testing()

if (MSVC)
    # 设置 msvc 输入代码编码格式为 utf-8
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt库
find_package(Qt6 REQUIRED COMPONENTS Widgets Bluetooth)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/)

add_subdirectory(src)
include_directories(inc)


qt_add_executable(Demo
    MANUAL_FINALIZATION
    src/main.cpp
    inc/ui/mainwindow.h
    inc/base/Bluetooth.hpp
    ${_UI_SOURCES}
    ${_BASE_SOURCES}
)

target_link_libraries(Demo PRIVATE Qt6::Widgets Qt6::Bluetooth)

set_target_properties(Demo PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(Demo)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
