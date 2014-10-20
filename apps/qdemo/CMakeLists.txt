cmake_minimum_required(VERSION 2.8.11)

project(pfcpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Qml)
find_package(Qt5Widgets)
find_package(Qt5Quick)

add_executable(qdemo main.cpp
					 SimulationModel.cpp
					 SensorSettingsModel.cpp
					 MotionModel.cpp)

target_link_libraries(qdemo Qt5::Widgets)
target_link_libraries(qdemo Qt5::Qml)
target_link_libraries(qdemo pfcpp)

add_custom_command(TARGET qdemo
	POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.qml ${CMAKE_CURRENT_BINARY_DIR}/mainwindow.qml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/SensorSettings.qml ${CMAKE_CURRENT_BINARY_DIR}/SensorSettings.qml
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/MotionSettings.qml ${CMAKE_CURRENT_BINARY_DIR}/MotionSettings.qml        
    COMMENT "Copying QML files")