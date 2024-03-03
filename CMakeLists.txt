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

get_filename_component(JUSTONE_ASSETS_LOCATION assets ABSOLUTE)
configure_file(src/assets_location.h.in assets_location.h @ONLY)

add_executable(justone
  src/capabilities.cc
  src/capabilities.h
  src/context.cc
  src/context.h
  src/playground_test.cc
  src/playground_test.h
  src/image_decoder.cc
  src/image_decoder.h
  src/swapchain.cc
  src/swapchain.h
  src/unittests.cc
  src/vk.h
)

target_include_directories(justone
  PUBLIC
    third_party/vulkan_headers/include
    third_party/stb
    src
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(justone
  PUBLIC
    gtest_main
    fml
    glfw
    Rpcrt4.lib
    Shlwapi.lib
    Winmm.lib
)
