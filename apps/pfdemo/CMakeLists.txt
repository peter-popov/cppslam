cmake_minimum_required(VERSION 2.8.11)

project(pfcpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_BUILD_TYPE Debug)

find_package(Qt5Widgets)

add_executable(pfdemo main.cpp mainwindow.cpp)

target_link_libraries(pfdemo Qt5::Widgets)
target_link_libraries(pfdemo pfcpp)