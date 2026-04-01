cmake_minimum_required(VERSION 3.20)
project(lesson_15_parameters)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_library(robot_controller_lib src/robot_controller.cpp)
target_include_directories(robot_controller_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(robot_controller_lib rclcpp)

add_executable(robot_controller src/robot_controller_main.cpp)
target_link_libraries(robot_controller robot_controller_lib)

install(TARGETS robot_controller DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_robot_controller test/test_robot_controller.cpp)
  target_link_libraries(test_robot_controller robot_controller_lib)
  ament_target_dependencies(test_robot_controller rclcpp)
endif()

ament_package()
