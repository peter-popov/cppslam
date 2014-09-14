cmake_minimum_required(VERSION 2.8.11)

project(pfcpp)

set(BUILD_TESTING ON)

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

add_executable(pfcpp_test test_main.cpp resample_test.cpp motion_test.cpp flatworld_test.cpp)

target_link_libraries(pfcpp_test
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )

target_link_libraries(pfcpp_test pfcpp)

enable_testing()
add_test(pfcpp_test pfcpp_test)