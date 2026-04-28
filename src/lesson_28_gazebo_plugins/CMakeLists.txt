cmake_minimum_required(VERSION 3.20)
project(lesson_28_gazebo_plugins)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)

install(DIRECTORY urdf/ DESTINATION share/${PROJECT_NAME}/urdf)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(ament_index_cpp REQUIRED)
  find_package(urdf REQUIRED)

  ament_add_gtest(test_gazebo_urdf test/test_gazebo_urdf.cpp)
  ament_target_dependencies(test_gazebo_urdf ament_index_cpp urdf)

  # Description-only smoke test (full Gazebo simulation skipped on CI)
  ament_add_pytest_test(test_gazebo_simplebot_launch_smoke
    test/test_gazebo_simplebot_launch_smoke.py)
endif()

ament_package()
