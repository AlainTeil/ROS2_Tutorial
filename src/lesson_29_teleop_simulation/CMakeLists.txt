cmake_minimum_required(VERSION 3.20)
project(lesson_29_teleop_simulation)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

# --- Libraries ---
add_library(teleop_lib src/teleop_node.cpp)
target_include_directories(teleop_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(teleop_lib PUBLIC cxx_std_20)
ament_target_dependencies(teleop_lib rclcpp geometry_msgs)

add_library(sensor_display_lib src/sensor_display.cpp)
target_include_directories(sensor_display_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(sensor_display_lib PUBLIC cxx_std_20)
ament_target_dependencies(sensor_display_lib rclcpp sensor_msgs)

# --- Executables ---
add_executable(teleop_node src/teleop_node_main.cpp)
target_link_libraries(teleop_node teleop_lib)

add_executable(sensor_display_node src/sensor_display_main.cpp)
target_link_libraries(sensor_display_node sensor_display_lib)

install(TARGETS teleop_node sensor_display_node
  DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_teleop_node test/test_teleop_node.cpp)
  target_link_libraries(test_teleop_node teleop_lib)

  ament_add_gtest(test_sensor_display test/test_sensor_display.cpp)
  target_link_libraries(test_sensor_display sensor_display_lib)
endif()

ament_package()
