cmake_minimum_required(VERSION 3.10)
project(pmrspy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
option(USE_SYSTEM_FMT "Use system-installed fmt library if available" ON)

if(USE_SYSTEM_FMT)
    find_package(fmt QUIET)
endif()

if(NOT fmt_FOUND)
  message(STATUS "Downloading fmt library...")
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.1.1  # Use a specific version
  )
  FetchContent_MakeAvailable(fmt)
  message(STATUS "fmt library downloaded and configured")
else()
    message(STATUS "Using system-installed fmt library")
endif()

add_library(pmrspy INTERFACE)
add_library(pmrspy::pmrspy ALIAS pmrspy)

target_include_directories(pmrspy INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(pmrspy INTERFACE fmt::fmt)

install(TARGETS pmrspy
    EXPORT pmrspy-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES pmrspy.hpp
    DESTINATION include/pmrspy
)

install(EXPORT pmrspy-targets
    FILE pmrspy-config.cmake
    NAMESPACE pmrspy::
    DESTINATION lib/cmake/pmrspy
)

option(BUILD_EXAMPLES "Build example applications" ON)

if(BUILD_EXAMPLES)
    add_executable(print_example ./examples/example.cpp)
    target_link_libraries(print_example PRIVATE pmrspy)
endif()
