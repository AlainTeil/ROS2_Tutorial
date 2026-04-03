cmake_minimum_required(VERSION 3.20)
project(lesson_22_tf2_practice)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# --- Library ---
add_library(frame_transformer_lib src/frame_transformer.cpp)
target_include_directories(frame_transformer_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(frame_transformer_lib PUBLIC cxx_std_20)
ament_target_dependencies(frame_transformer_lib
  rclcpp tf2 tf2_ros tf2_geometry_msgs geometry_msgs)

# --- Executable ---
add_executable(frame_transformer_node src/frame_transformer_main.cpp)
target_link_libraries(frame_transformer_node frame_transformer_lib)

install(TARGETS frame_transformer_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_frame_transformer test/test_frame_transformer.cpp)
  target_link_libraries(test_frame_transformer frame_transformer_lib)
endif()

ament_package()
