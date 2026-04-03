cmake_minimum_required(VERSION 3.20)
project(lesson_26_robot_state_publisher)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

# --- Library ---
add_library(wheel_joint_publisher_lib src/wheel_joint_publisher.cpp)
target_include_directories(wheel_joint_publisher_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(wheel_joint_publisher_lib PUBLIC cxx_std_20)
ament_target_dependencies(wheel_joint_publisher_lib rclcpp sensor_msgs)

# --- Executable ---
add_executable(wheel_joint_publisher_node src/wheel_joint_publisher_main.cpp)
target_link_libraries(wheel_joint_publisher_node wheel_joint_publisher_lib)

install(TARGETS wheel_joint_publisher_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY rviz/ DESTINATION share/${PROJECT_NAME}/rviz)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_wheel_joint_publisher test/test_wheel_joint_publisher.cpp)
  target_link_libraries(test_wheel_joint_publisher wheel_joint_publisher_lib)
endif()

ament_package()
