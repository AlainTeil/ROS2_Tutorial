cmake_minimum_required(VERSION 3.20)
project(lesson_13_actions_server)

# — Compiler settings --------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# — Dependencies --------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# — Action generation ---------------------------------------------------------
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/NavigateToPoint.action"
)

# — Library -------------------------------------------------------------------
add_library(navigate_action_server_lib src/navigate_action_server.cpp)
target_include_directories(navigate_action_server_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(navigate_action_server_lib rclcpp rclcpp_action)
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME}
  "rosidl_typesupport_cpp")
target_link_libraries(navigate_action_server_lib "${cpp_typesupport_target}")

# — Executable ----------------------------------------------------------------
add_executable(navigate_action_server src/navigate_action_server_main.cpp)
target_link_libraries(navigate_action_server navigate_action_server_lib)

# — Install -------------------------------------------------------------------
install(TARGETS navigate_action_server DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_navigate_action_server
    test/test_navigate_action_server.cpp)
  target_link_libraries(test_navigate_action_server
    navigate_action_server_lib)
  ament_target_dependencies(test_navigate_action_server
    rclcpp rclcpp_action)
endif()

ament_package()
