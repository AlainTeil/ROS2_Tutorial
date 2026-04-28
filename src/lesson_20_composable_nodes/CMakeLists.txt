cmake_minimum_required(VERSION 3.20)
project(lesson_20_composable_nodes)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)

# --- Shared library for components ---
add_library(composable_nodes SHARED
  src/composable_publisher.cpp
  src/composable_subscriber.cpp)
target_include_directories(composable_nodes PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(composable_nodes PUBLIC cxx_std_20)
ament_target_dependencies(composable_nodes
  rclcpp rclcpp_components std_msgs)

# Register components so the container can find them
rclcpp_components_register_nodes(composable_nodes
  "lesson_20::ComposablePublisher"
  "lesson_20::ComposableSubscriber")

install(TARGETS composable_nodes
  EXPORT export_composable_nodes
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

# Export the library so downstream packages (and tooling) can find it
# via `ament_target_dependencies(... lesson_20_composable_nodes)`. The
# component loader does not strictly need this, but exporting is the
# documented pattern for shared libraries built with ament_cmake.
ament_export_targets(export_composable_nodes HAS_LIBRARY_TARGET)
ament_export_libraries(composable_nodes)
ament_export_dependencies(rclcpp rclcpp_components std_msgs)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(launch_testing_ament_cmake REQUIRED)

  ament_add_gtest(test_composable_publisher test/test_composable_publisher.cpp)
  target_link_libraries(test_composable_publisher composable_nodes)

  ament_add_gtest(test_composable_subscriber test/test_composable_subscriber.cpp)
  target_link_libraries(test_composable_subscriber composable_nodes)

  add_launch_test(test/test_composable_launch.py)
endif()

ament_package()
