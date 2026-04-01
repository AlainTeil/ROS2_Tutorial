cmake_minimum_required(VERSION 3.20)
project(lesson_25_xacro_urdf)

find_package(ament_cmake REQUIRED)

install(DIRECTORY urdf/ DESTINATION share/${PROJECT_NAME}/urdf)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_index_cpp REQUIRED)
  find_package(urdf REQUIRED)

  ament_add_gtest(test_xacro_generation test/test_xacro_generation.cpp)
  ament_target_dependencies(test_xacro_generation ament_index_cpp urdf)
endif()

ament_package()
