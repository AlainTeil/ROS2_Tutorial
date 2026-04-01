cmake_minimum_required(VERSION 3.20)
project(lesson_08_topic_subscriber)

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
add_library(number_subscriber_lib
  src/number_subscriber.cpp
)
target_include_directories(number_subscriber_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(number_subscriber_lib rclcpp std_msgs)

# — Executable ----------------------------------------------------------------
add_executable(number_subscriber src/number_subscriber_main.cpp)
target_link_libraries(number_subscriber number_subscriber_lib)

# — Install -------------------------------------------------------------------
install(TARGETS number_subscriber DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)

# — Tests ---------------------------------------------------------------------
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_number_subscriber test/test_number_subscriber.cpp)
  target_link_libraries(test_number_subscriber number_subscriber_lib)
  ament_target_dependencies(test_number_subscriber rclcpp std_msgs)
endif()

ament_package()
