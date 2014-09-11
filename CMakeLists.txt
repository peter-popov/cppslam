cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
add_definitions(-std=c++1y)
#add_definitions(-stdlib=libc++)

#find_package( Boost 1.54 COMPONENTS geometry REQUIRED )

project (mcl_test)

add_executable(mcl_test mcl.cpp)

add_subdirectory(samples/pfdemo)