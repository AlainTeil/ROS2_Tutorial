cmake_minimum_required(VERSION 3.20)
project(lesson_03_cpp20_tooling)

# --------------------------------------------------------------------------- #
# Compiler configuration
# --------------------------------------------------------------------------- #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

# --------------------------------------------------------------------------- #
# Dependencies
# --------------------------------------------------------------------------- #
find_package(ament_cmake REQUIRED)

# --------------------------------------------------------------------------- #
# Library
# --------------------------------------------------------------------------- #
add_library(cpp20_showcase_lib STATIC
  src/cpp20_showcase.cpp
)
target_include_directories(cpp20_showcase_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# --------------------------------------------------------------------------- #
# Executable
# --------------------------------------------------------------------------- #
add_executable(cpp20_showcase_demo src/cpp20_showcase_main.cpp)
target_link_libraries(cpp20_showcase_demo PRIVATE cpp20_showcase_lib)

install(TARGETS cpp20_showcase_demo
  DESTINATION lib/${PROJECT_NAME}
)

# --------------------------------------------------------------------------- #
# Tests
# --------------------------------------------------------------------------- #
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_cpp20_showcase test/test_cpp20_showcase.cpp)
  target_link_libraries(test_cpp20_showcase cpp20_showcase_lib)
endif()

ament_package()
