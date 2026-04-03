cmake_minimum_required(VERSION 3.20)
project(lesson_20_composable_nodes)
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
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_composable_publisher test/test_composable_publisher.cpp)
  target_link_libraries(test_composable_publisher composable_nodes)

  ament_add_gtest(test_composable_subscriber test/test_composable_subscriber.cpp)
  target_link_libraries(test_composable_subscriber composable_nodes)
endif()

ament_package()
