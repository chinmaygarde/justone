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

enable_testing()
include(GoogleTest)

add_executable(justone
  src/unittests.cc
)

target_link_libraries(justone
  PRIVATE
    gtest_main
)
