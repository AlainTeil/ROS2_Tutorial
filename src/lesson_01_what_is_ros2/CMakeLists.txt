cmake_minimum_required(VERSION 3.20)
project(lesson_01_what_is_ros2)

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

# --------------------------------------------------------------------------- #
# Library (shared logic for executable and tests)
# --------------------------------------------------------------------------- #
add_library(hello_ros2_world_lib STATIC
  src/hello_ros2_world.cpp
)
target_include_directories(hello_ros2_world_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# --------------------------------------------------------------------------- #
# Executable
# --------------------------------------------------------------------------- #
add_executable(hello_ros2_world src/hello_ros2_world_main.cpp)
target_link_libraries(hello_ros2_world PRIVATE hello_ros2_world_lib)

install(TARGETS hello_ros2_world
  DESTINATION lib/${PROJECT_NAME}
)

# --------------------------------------------------------------------------- #
# Tests
# --------------------------------------------------------------------------- #
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_hello_ros2_world test/test_hello_ros2_world.cpp)
  target_link_libraries(test_hello_ros2_world hello_ros2_world_lib)
endif()

ament_package()
