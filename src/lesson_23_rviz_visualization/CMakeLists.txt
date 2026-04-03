cmake_minimum_required(VERSION 3.20)
project(lesson_23_rviz_visualization)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# --- Library ---
add_library(marker_publisher_lib src/marker_publisher.cpp)
target_include_directories(marker_publisher_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(marker_publisher_lib PUBLIC cxx_std_20)
ament_target_dependencies(marker_publisher_lib
  rclcpp visualization_msgs geometry_msgs)

# --- Executable ---
add_executable(marker_publisher_node src/marker_publisher_main.cpp)
target_link_libraries(marker_publisher_node marker_publisher_lib)

install(TARGETS marker_publisher_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY rviz/ DESTINATION share/${PROJECT_NAME}/rviz)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_marker_publisher test/test_marker_publisher.cpp)
  target_link_libraries(test_marker_publisher marker_publisher_lib)
endif()

ament_package()
