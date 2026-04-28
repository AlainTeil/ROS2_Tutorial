cmake_minimum_required(VERSION 3.20)
project(lesson_30_gazebo_world)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)

install(DIRECTORY worlds/ DESTINATION share/${PROJECT_NAME}/worlds)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(test_gazebo_arena_launch_smoke
    test/test_gazebo_arena_launch_smoke.py)
endif()

ament_package()
