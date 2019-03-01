cmake_minimum_required(VERSION 3.10.0)
project(git-r-done)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Update name and srcs
# add_library(example_lib src/example_lib.cc)

add_library(request src/request.cc)
add_library(server src/server.cc)
add_library(session src/session.cc)
add_library(config_parser src/config_parser.cc)
add_library(response src/response.cc)
add_library(echo_handler src/echo_handler.cc)
add_library(error_handler src/error_handler.cc)
add_library(static_handler src/static_handler.cc)
add_library(status_handler src/status_handler.cc)
add_library(proxy_handler src/proxy_handler.cc)
add_library(dispatcher src/dispatcher.cc)
add_library(handler_manager src/handler_manager.cc)
add_library(status_obj src/status_obj.cc)

# Update executable name, srcs, and deps
# add_executable(example_server src/example_server_main.cc)
# target_link_libraries(example_server example_lib Boost::system)

add_executable(server_main src/server_main.cc)
target_link_libraries(server_main server session dispatcher handler_manager echo_handler error_handler static_handler status_handler proxy_handler request config_parser response status_obj Boost::system Boost::log_setup Boost::log)

# Update test executable name, srcs, and deps
# add_executable(example_lib_test tests/example_lib_test.cc)
# target_link_libraries(example_lib_test example_lib gtest_main)
