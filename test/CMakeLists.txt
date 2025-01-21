# Reflector: A simple struct reflection framework for C++17.
# @file CMake tests configuration file.
# @author Rodrigo Siqueira <rodriados@gmail.com>
# @copyright 2025-present Rodrigo Siqueira
cmake_minimum_required(VERSION 3.24)

find_package(Catch2 REQUIRED)

# The test sources should be automatically discovered by iterating over the directory.
# By doing this, we avoid the need to manually add new tests to this file.
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

# Configure the test target and makes it available as an executable. This executable
# should be responsible for running all tests listed in the repository.
add_executable(ReflectorTest ${TEST_SOURCES})
target_link_libraries(ReflectorTest PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
target_include_directories(ReflectorTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)

# Automatically discovers the tests found on the repository and allows them to be
# called from and linked to ctest.
catch_discover_tests(ReflectorTest)
