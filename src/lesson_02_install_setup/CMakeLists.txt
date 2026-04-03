cmake_minimum_required(VERSION 3.20)
project(lesson_02_install_setup)

# --------------------------------------------------------------------------- #
# Compiler configuration
# --------------------------------------------------------------------------- #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# --------------------------------------------------------------------------- #
# Dependencies
# --------------------------------------------------------------------------- #
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# --------------------------------------------------------------------------- #
# Library
# --------------------------------------------------------------------------- #
add_library(check_ros2_env_lib STATIC
  src/check_ros2_env.cpp
)
target_include_directories(check_ros2_env_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(check_ros2_env_lib rclcpp)

# --------------------------------------------------------------------------- #
# Executable
# --------------------------------------------------------------------------- #
add_executable(check_ros2_env src/check_ros2_env_main.cpp)
target_link_libraries(check_ros2_env PRIVATE check_ros2_env_lib)

install(TARGETS check_ros2_env
  DESTINATION lib/${PROJECT_NAME}
)

# --------------------------------------------------------------------------- #
# Tests
# --------------------------------------------------------------------------- #
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_check_ros2_env test/test_check_ros2_env.cpp)
  target_link_libraries(test_check_ros2_env check_ros2_env_lib)
  ament_target_dependencies(test_check_ros2_env rclcpp)
endif()

ament_package()
