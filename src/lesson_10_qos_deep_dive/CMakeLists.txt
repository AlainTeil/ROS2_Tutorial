cmake_minimum_required(VERSION 3.20)
project(lesson_10_qos_deep_dive)

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
add_library(qos_demo_lib src/qos_demo.cpp)
target_include_directories(qos_demo_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(qos_demo_lib rclcpp std_msgs)

# — Executable ----------------------------------------------------------------
add_executable(qos_demo src/qos_demo_main.cpp)
target_link_libraries(qos_demo qos_demo_lib)

# — Install -------------------------------------------------------------------
install(TARGETS qos_demo DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_qos_demo test/test_qos_demo.cpp)
  target_link_libraries(test_qos_demo qos_demo_lib)
  ament_target_dependencies(test_qos_demo rclcpp std_msgs)
endif()

ament_package()
