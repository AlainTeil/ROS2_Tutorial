cmake_minimum_required(VERSION 3.20)
project(lesson_12_service_client)

# — Compiler settings --------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# — Dependencies --------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED)

# — Library -------------------------------------------------------------------
add_library(add_two_ints_client_lib src/add_two_ints_client.cpp)
target_include_directories(add_two_ints_client_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(add_two_ints_client_lib rclcpp example_interfaces)

# — Executable ----------------------------------------------------------------
add_executable(add_two_ints_client src/add_two_ints_client_main.cpp)
target_link_libraries(add_two_ints_client add_two_ints_client_lib)

# — Install -------------------------------------------------------------------
install(TARGETS add_two_ints_client DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_add_two_ints_client test/test_add_two_ints_client.cpp)
  target_link_libraries(test_add_two_ints_client add_two_ints_client_lib)
  ament_target_dependencies(test_add_two_ints_client rclcpp example_interfaces)
endif()

ament_package()
