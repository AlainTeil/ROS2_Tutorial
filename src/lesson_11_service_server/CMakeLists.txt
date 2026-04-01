cmake_minimum_required(VERSION 3.20)
project(lesson_11_service_server)

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
find_package(rosidl_default_generators REQUIRED)

# — Service generation --------------------------------------------------------
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/ComputeTrajectory.srv"
)

# — Library -------------------------------------------------------------------
add_library(service_server_lib src/add_two_ints_server.cpp)
target_include_directories(service_server_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(service_server_lib rclcpp example_interfaces)
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME}
  "rosidl_typesupport_cpp")
target_link_libraries(service_server_lib "${cpp_typesupport_target}")

# — Executable ----------------------------------------------------------------
add_executable(add_two_ints_server src/add_two_ints_server_main.cpp)
target_link_libraries(add_two_ints_server service_server_lib)

# — Install -------------------------------------------------------------------
install(TARGETS add_two_ints_server DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_add_two_ints_server test/test_add_two_ints_server.cpp)
  target_link_libraries(test_add_two_ints_server service_server_lib)
  ament_target_dependencies(test_add_two_ints_server rclcpp example_interfaces)
endif()

ament_package()
