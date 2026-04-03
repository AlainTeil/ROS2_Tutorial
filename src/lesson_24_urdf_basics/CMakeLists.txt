cmake_minimum_required(VERSION 3.20)
project(lesson_24_urdf_basics)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)

# No C++ library or executables — URDF-only package.
install(DIRECTORY urdf/ DESTINATION share/${PROJECT_NAME}/urdf)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_index_cpp REQUIRED)
  find_package(urdf REQUIRED)

  ament_add_gtest(test_urdf_parsing test/test_urdf_parsing.cpp)
  ament_target_dependencies(test_urdf_parsing ament_index_cpp urdf)
endif()

ament_package()
