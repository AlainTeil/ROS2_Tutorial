cmake_minimum_required(VERSION 3.20)
project(lesson_21_tf2_concepts)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)

# --- Library ---
add_library(tf_nodes_lib
  src/tf_broadcaster.cpp
  src/tf_listener.cpp)
target_include_directories(tf_nodes_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(tf_nodes_lib PUBLIC cxx_std_20)
ament_target_dependencies(tf_nodes_lib
  rclcpp tf2 tf2_ros geometry_msgs)

# --- Executables ---
add_executable(tf_broadcaster_node src/tf_broadcaster_main.cpp)
target_link_libraries(tf_broadcaster_node tf_nodes_lib)

add_executable(tf_listener_node src/tf_listener_main.cpp)
target_link_libraries(tf_listener_node tf_nodes_lib)

install(TARGETS tf_broadcaster_node tf_listener_node
  DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_tf_broadcaster test/test_tf_broadcaster.cpp)
  target_link_libraries(test_tf_broadcaster tf_nodes_lib)
endif()

ament_package()
