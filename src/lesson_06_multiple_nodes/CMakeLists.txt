cmake_minimum_required(VERSION 3.20)
project(lesson_06_multiple_nodes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Library
add_library(multi_nodes_lib STATIC
  src/sensor_node.cpp
  src/processor_node.cpp
)
target_include_directories(multi_nodes_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(multi_nodes_lib rclcpp)

# Executable
add_executable(multi_node_executor src/multi_node_executor.cpp)
target_link_libraries(multi_node_executor PRIVATE multi_nodes_lib)

install(TARGETS multi_node_executor DESTINATION lib/${PROJECT_NAME})

# Tests
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_sensor_node test/test_sensor_node.cpp)
  target_link_libraries(test_sensor_node multi_nodes_lib)
  ament_target_dependencies(test_sensor_node rclcpp)

  ament_add_gtest(test_processor_node test/test_processor_node.cpp)
  target_link_libraries(test_processor_node multi_nodes_lib)
  ament_target_dependencies(test_processor_node rclcpp)
endif()

ament_package()
