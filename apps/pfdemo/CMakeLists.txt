cmake_minimum_required(VERSION 2.8.11)

project(pfdemo)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
add_definitions(-std=c++1y)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

add_executable(pfdemo main.cpp mainwindow.cpp)


target_link_libraries(pfdemo Qt5::Widgets)