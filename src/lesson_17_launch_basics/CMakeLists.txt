cmake_minimum_required(VERSION 3.20)
project(lesson_17_launch_basics)

find_package(ament_cmake REQUIRED)

# Install launch files
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

ament_package()
