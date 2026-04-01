cmake_minimum_required(VERSION 3.20)
project(lesson_09_custom_messages)

# — Compiler settings --------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# — Dependencies --------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# — Message generation --------------------------------------------------------
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/SensorReading.msg"
  DEPENDENCIES builtin_interfaces
)

# — Library (publisher) -------------------------------------------------------
add_library(sensor_publisher_lib src/sensor_publisher.cpp)
target_include_directories(sensor_publisher_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(sensor_publisher_lib rclcpp)
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME}
  "rosidl_typesupport_cpp")
target_link_libraries(sensor_publisher_lib "${cpp_typesupport_target}")

# — Library (subscriber) ------------------------------------------------------
add_library(sensor_subscriber_lib src/sensor_subscriber.cpp)
target_include_directories(sensor_subscriber_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(sensor_subscriber_lib rclcpp)
target_link_libraries(sensor_subscriber_lib "${cpp_typesupport_target}")

# — Executables ---------------------------------------------------------------
add_executable(sensor_publisher src/sensor_publisher_main.cpp)
target_link_libraries(sensor_publisher sensor_publisher_lib)

add_executable(sensor_subscriber src/sensor_subscriber_main.cpp)
target_link_libraries(sensor_subscriber sensor_subscriber_lib)

# — Install -------------------------------------------------------------------
install(TARGETS sensor_publisher sensor_subscriber
  DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_sensor_reading_msg test/test_sensor_reading_msg.cpp)
  target_link_libraries(test_sensor_reading_msg
    sensor_publisher_lib sensor_subscriber_lib)
  ament_target_dependencies(test_sensor_reading_msg rclcpp)
endif()

ament_package()
