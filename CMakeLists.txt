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

# TODO(!): Update name and srcs
# add_library(example_lib src/example_lib.cc)

add_library(request src/request.cc)
add_library(server src/server.cc)
add_library(session src/session.cc)
add_library(config_parser src/config_parser.cc)

# TODO(!): Update executable name, srcs, and deps
# add_executable(example_server src/example_server_main.cc)
# target_link_libraries(example_server example_lib Boost::system)

add_executable(server_main src/server_main.cc)
target_link_libraries(server_main server session request config_parser Boost::system Boost::log_setup Boost::log)

# TODO(!): Update test executable name, srcs, and deps
# add_executable(example_lib_test tests/example_lib_test.cc)
# target_link_libraries(example_lib_test example_lib gtest_main)

add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser gtest_main Boost::log_setup Boost::log)

add_executable(request_test tests/request_test.cc)
target_link_libraries(request_test server session request config_parser gtest_main Boost::log_setup Boost::log)

add_executable(echo_test tests/echo_test.cc)
target_link_libraries(echo_test server session request config_parser gtest_main Boost::system Boost::log_setup Boost::log)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server session request config_parser gtest_main Boost::system Boost::log_setup Boost::log)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test server session request config_parser gtest_main Boost::system Boost::log_setup Boost::log)

# TODO(!): Update with test binary
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_test /usr/src/projects/git-r-done build/googletest)
gtest_discover_tests(request_test /usr/src/projects/git-r-done build/googletest)
gtest_discover_tests(echo_test /usr/src/projects/git-r-done build/googletest)
gtest_discover_tests(server_test /usr/src/projects/git-r-done build/googletest)
gtest_discover_tests(session_test /usr/src/projects/git-r-done build/googletest)

# TODO(!): Update with target/test targets
# include(cmake/CodeCoverageReportConfig.cmake)
#generate_coverage_report(TARGETS example_server example_lib TESTS example_lib_test)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server_main server session request config_parser TESTS request_test echo_test server_test session_test config_parser_test)

#Add integration test to cmake 
#add_test(NAME Integration COMMAND bash integration_test.sh WORKING_DIRECTORY /usr/src/project/tests)
