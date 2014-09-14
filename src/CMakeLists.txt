cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(pfcpp)
add_definitions(-std=c++1y)


add_library(pfcpp SHARED map/shapes_map.cpp)