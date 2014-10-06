cmake_minimum_required(VERSION 2.8.11)

project(pfcpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Qml)
find_package(Qt5Widgets)
find_package(Qt5Quick)

add_executable(qdemo main.cpp
					 SimulationModel.cpp)

target_link_libraries(qdemo Qt5::Widgets)
target_link_libraries(qdemo Qt5::Qml)
target_link_libraries(qdemo pfcpp)

file(COPY mainwindow.qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
file(COPY SensorSettings.qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )