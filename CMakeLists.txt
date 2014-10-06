cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(pfcpp)
add_definitions(-std=c++1y)

include_directories("./include")

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(apps/pfdemo)
add_subdirectory(apps/qdemo)