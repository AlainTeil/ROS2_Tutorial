cmake_minimum_required(VERSION 3.20)
project(lesson_27_gazebo_intro)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)

install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY worlds/ DESTINATION share/${PROJECT_NAME}/worlds)

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  # Description-only smoke test (full Gazebo simulation skipped on CI)
  ament_add_pytest_test(test_gazebo_empty_world_launch_smoke
    test/test_gazebo_empty_world_launch_smoke.py)
endif()

ament_package()
