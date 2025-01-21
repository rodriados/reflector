# Reflector: A simple struct reflection framework for C++17.
# @file CMake configuration file.
# @author Rodrigo Siqueira <rodriados@gmail.com>
# @copyright 2025-present Rodrigo Siqueira
cmake_minimum_required(VERSION 3.24)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(HelperFunctions)

# Discovers the project's version and makes it readily available in a variable.
# The version is sourced from the code itself.
get_project_version(REFLECTOR_VERSION "src/reflector/version.h")

# Determines whether the current project is the master target or it is being built
# as a dependency, potentially as a thirdparty, to an external project.
if(NOT DEFINED REFLECTOR_MASTER_PROJECT)
  set(REFLECTOR_MASTER_PROJECT OFF)
  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(REFLECTOR_MASTER_PROJECT ON)
    message(STATUS "Project: Reflector ${REFLECTOR_VERSION}")
    message(STATUS "CMake version: ${CMAKE_VERSION}")
  endif()
endif()

# Command line options to control the conditions and generation of the targets.
# The targets may be enabled on demand, depending on the user or environment needs.
option(REFLECTOR_INSTALL "Generate the install target." ON)

cmake_dependent_option(REFLECTOR_DEVELOPMENT_BUILD "Build tests, enable warnings and errors." ON "BUILD_TESTING" OFF)
cmake_dependent_option(REFLECTOR_BUILD_TESTING "Generate the test targets." ON "BUILD_TESTING" OFF)
cmake_dependent_option(REFLECTOR_ENABLE_PEDANTIC "Errors on warnings." ON "BUILD_TESTING" OFF)

# Discovers the project's current version and initializes it. The version is retrieved
# from a source file where the version should be explicitly set.
project(
  Reflector
    LANGUAGES CXX
    VERSION ${REFLECTOR_VERSION}
    HOMEPAGE_URL "https://github.com/rodriados/reflector"
    DESCRIPTION "A simple struct reflection framework for C++17.")

include(GNUInstallDirs)

# Installation, configuration and targets variables. These variables are used to
# define where the project must be installed on and where to store its config files.
set(REFLECTOR_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Reflector")
set(REFLECTOR_INCLUDE_DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/reflector")
set(REFLECTOR_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(REFLECTOR_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/ReflectorConfigVersion.cmake")
set(REFLECTOR_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/ReflectorConfig.cmake")
set(REFLECTOR_CONFIG_FILE_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/cmake/ReflectorConfig.cmake.in")
set(REFLECTOR_TARGETS_EXPORT_NAME "ReflectorTargets")

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

# Include third-party dependencies. These packages are required for the functioning
# of the current project. Therefore, we must bail out if they are not found.
find_package(SuperTuple REQUIRED)

# As we define our library as header-only, we must allow all header files to be
# found and eventually exported and installed in the user machine.
add_library(Reflector INTERFACE)
add_library(Reflector::Reflector ALIAS Reflector)

target_compile_features(Reflector INTERFACE cxx_std_17)
target_include_directories(
  Reflector INTERFACE
    $<BUILD_INTERFACE:${REFLECTOR_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(Reflector INTERFACE SuperTuple)

# Sets the compiler flags for a pedantic compilation. We mostly assume that the
# used compiler is GNU-compatible, although some special cases might be cared for.
if(REFLECTOR_ENABLE_PEDANTIC)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-pedantic-errors -Wall -Wextra -pedantic -Werror)
  elseif(MSVC)
    add_compile_options(/W3 /WX)
  endif()
endif()

# Installs the library by copying all needed files to their corresponding destination
# folders. The library should be installed globally in the user system.
if(REFLECTOR_INSTALL)
  install(
    TARGETS Reflector
    EXPORT ReflectorTargets
    RUNTIME_DEPENDENCY_SET ReflectorDependencies
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

  export(
    TARGETS Reflector
    NAMESPACE Reflector::
    FILE "${PROJECT_BINARY_DIR}/ReflectorTargets.cmake")

  install(
    EXPORT ReflectorTargets
    NAMESPACE Reflector::
    DESTINATION ${REFLECTOR_CMAKE_CONFIG_DESTINATION})

  install(
    DIRECTORY "${REFLECTOR_SOURCE_DIR}/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

  configure_package_config_file(
    ${REFLECTOR_CONFIG_FILE_TEMPLATE}
    ${REFLECTOR_CONFIG_FILE}
    INSTALL_DESTINATION ${REFLECTOR_CMAKE_CONFIG_DESTINATION})

  write_basic_package_version_file(
    ${REFLECTOR_VERSION_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  install(
    FILES
      ${REFLECTOR_CONFIG_FILE}
      ${REFLECTOR_VERSION_FILE}
    DESTINATION ${REFLECTOR_CMAKE_CONFIG_DESTINATION})
endif()

# Declaration of targets for testing. These targets are only available when this
# project is root, therefore not used as dependency for any other.
if(BUILD_TESTING AND REFLECTOR_MASTER_PROJECT AND REFLECTOR_BUILD_TESTING)
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif()

# Declaration of target for bundling and packing the project's source code to make
# it possible to include the whole library as a single header-only file.
if(REFLECTOR_MASTER_PROJECT)
  include(PackSourceCode)
endif()
