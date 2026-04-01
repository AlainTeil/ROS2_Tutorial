cmake_minimum_required(VERSION 3.20)
project(lesson_04_core_concepts)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# Library
add_library(concepts_explorer_lib STATIC
  src/concepts_explorer.cpp
)
target_include_directories(concepts_explorer_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(concepts_explorer_lib rclcpp)

# Executable
add_executable(concepts_explorer src/concepts_explorer_main.cpp)
target_link_libraries(concepts_explorer PRIVATE concepts_explorer_lib)

install(TARGETS concepts_explorer DESTINATION lib/${PROJECT_NAME})

# Tests
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_concepts_explorer test/test_concepts_explorer.cpp)
  target_link_libraries(test_concepts_explorer concepts_explorer_lib)
  ament_target_dependencies(test_concepts_explorer rclcpp)
endif()

ament_package()
