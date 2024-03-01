cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM NAMES ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

project(justone)

add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)
add_subdirectory(third_party/fml EXCLUDE_FROM_ALL)
add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL)

enable_testing()
include(GoogleTest)

add_compile_definitions(
  _CRT_SECURE_NO_WARNINGS
  VULKAN_HPP_NO_SPACESHIP_OPERATOR
)

add_executable(justone
  src/unittests.cc
  src/context.cc
  src/context.h
  src/macros.h
  src/playground_test.h
  src/playground_test.cc
  src/vk.h
  src/caps.h
  src/caps.cc
)

target_include_directories(justone
  PUBLIC
    third_party/vulkan_headers/include
    src
)

target_link_libraries(justone
  PUBLIC
    gtest_main
    fml
    glfw
)
