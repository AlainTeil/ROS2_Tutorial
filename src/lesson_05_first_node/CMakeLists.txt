cmake_minimum_required(VERSION 3.20)
project(lesson_05_first_node)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Library
add_library(first_node_lib STATIC src/first_node.cpp)
target_include_directories(first_node_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(first_node_lib rclcpp)

# Executable
add_executable(first_node src/first_node_main.cpp)
target_link_libraries(first_node PRIVATE first_node_lib)

install(TARGETS first_node DESTINATION lib/${PROJECT_NAME})

# Tests
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_first_node test/test_first_node.cpp)
  target_link_libraries(test_first_node first_node_lib)
  ament_target_dependencies(test_first_node rclcpp)
endif()

ament_package()
