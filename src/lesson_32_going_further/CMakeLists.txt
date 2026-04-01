cmake_minimum_required(VERSION 3.20)
project(lesson_32_going_further)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(example_interfaces REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(lesson_13_actions_server REQUIRED)

# --- Library ---
add_library(capstone_node_lib
  src/capstone_node.cpp
)
target_include_directories(capstone_node_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(capstone_node_lib
  rclcpp
  rclcpp_lifecycle
  rclcpp_action
  std_msgs
  geometry_msgs
  example_interfaces
  tf2_ros
  lesson_13_actions_server
)

# --- Executable ---
add_executable(capstone_robot_node src/capstone_node_main.cpp)
target_link_libraries(capstone_robot_node capstone_node_lib)

# --- Install ---
install(TARGETS capstone_node_lib
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(TARGETS capstone_robot_node
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY include/
  DESTINATION include
)
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

# --- Tests ---
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_capstone_node
    test/test_capstone_node.cpp
  )
  target_link_libraries(test_capstone_node capstone_node_lib)
endif()

ament_package()
