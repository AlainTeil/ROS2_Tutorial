cmake_minimum_required(VERSION 3.20)
project(lesson_19_lifecycle_nodes)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(std_msgs REQUIRED)
find_package(lifecycle_msgs REQUIRED)

# --- Library ---
add_library(lifecycle_sensor_lib
  src/lifecycle_sensor.cpp)
target_include_directories(lifecycle_sensor_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(lifecycle_sensor_lib PUBLIC cxx_std_20)
ament_target_dependencies(lifecycle_sensor_lib
  rclcpp rclcpp_lifecycle std_msgs lifecycle_msgs)

# --- Executable ---
add_executable(lifecycle_sensor_node src/lifecycle_sensor_main.cpp)
target_link_libraries(lifecycle_sensor_node lifecycle_sensor_lib)

install(TARGETS lifecycle_sensor_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_lifecycle_sensor test/test_lifecycle_sensor.cpp)
  target_link_libraries(test_lifecycle_sensor lifecycle_sensor_lib)
  ament_target_dependencies(test_lifecycle_sensor lifecycle_msgs)
endif()

ament_package()
