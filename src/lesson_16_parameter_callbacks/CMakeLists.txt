cmake_minimum_required(VERSION 3.20)
project(lesson_16_parameter_callbacks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_library(validated_controller_lib src/validated_controller.cpp)
target_include_directories(validated_controller_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
ament_target_dependencies(validated_controller_lib rclcpp)

add_executable(validated_controller src/validated_controller_main.cpp)
target_link_libraries(validated_controller validated_controller_lib)

install(TARGETS validated_controller DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_validated_controller test/test_validated_controller.cpp)
  target_link_libraries(test_validated_controller validated_controller_lib)
  ament_target_dependencies(test_validated_controller rclcpp)
endif()

ament_package()
