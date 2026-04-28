cmake_minimum_required(VERSION 3.20)
project(lesson_31_sensor_processing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

# --- Library ---
add_library(obstacle_avoidance_lib
  src/obstacle_avoidance.cpp
)
target_include_directories(obstacle_avoidance_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(obstacle_avoidance_lib
  rclcpp
  geometry_msgs
  sensor_msgs
)

# --- Executable ---
add_executable(obstacle_avoidance_node src/obstacle_avoidance_main.cpp)
target_link_libraries(obstacle_avoidance_node obstacle_avoidance_lib)

# --- Install ---
install(TARGETS obstacle_avoidance_lib
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(TARGETS obstacle_avoidance_node
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
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_gtest(test_obstacle_avoidance
    test/test_obstacle_avoidance.cpp
  )
  target_link_libraries(test_obstacle_avoidance obstacle_avoidance_lib)
  ament_add_pytest_test(test_autonomous_sim_launch_smoke
    test/test_autonomous_sim_launch_smoke.py)
endif()

ament_package()
