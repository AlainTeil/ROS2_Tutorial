cmake_minimum_required(VERSION 3.20)
project(lesson_07_topic_publisher)

# — Compiler settings --------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# — Dependencies --------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# — Library -------------------------------------------------------------------
add_library(number_publisher_lib
  src/number_publisher.cpp
)
target_include_directories(number_publisher_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(number_publisher_lib rclcpp std_msgs)

# — Executable ----------------------------------------------------------------
add_executable(number_publisher src/number_publisher_main.cpp)
target_link_libraries(number_publisher number_publisher_lib)

# — Install -------------------------------------------------------------------
install(TARGETS number_publisher DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_number_publisher test/test_number_publisher.cpp)
  target_link_libraries(test_number_publisher number_publisher_lib)
  ament_target_dependencies(test_number_publisher rclcpp std_msgs)
endif()

ament_package()
