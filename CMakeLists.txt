# ==============================================================================
#  Description: Main project orchestrator. Manages configuration options,
#               global build settings, and subdirectories.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

cmake_minimum_required(VERSION 3.28)

# ------------------------------------------------------------------------------
#  Modules & Helpers
# ------------------------------------------------------------------------------
set(FOUNDRY_BASE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${FOUNDRY_BASE_FOLDER}/cmake")

# ------------------------------------------------------------------------------
#  Version
# ------------------------------------------------------------------------------
include(foundry_version)
foundry_load_version(FOUNDRY_VERSION FOUNDRY_PLAIN_VERSION)

project(Foundry
  LANGUAGES   CXX
  VERSION     ${FOUNDRY_PLAIN_VERSION}
  DESCRIPTION "Cross-platform C++ utility library."
)
message(STATUS "${PROJECT_NAME} version: ${FOUNDRY_VERSION}")
include("${FOUNDRY_BASE_FOLDER}/cmake/helpers/build_helpers.cmake")

# ------------------------------------------------------------------------------
#  Configuration Options
# ------------------------------------------------------------------------------

option(FOUNDRY_BUILD_TESTS         "Build Foundry's unit-test suite."            ${PROJECT_IS_TOP_LEVEL})
option(FOUNDRY_BUILD_EXAMPLES      "Build Foundry's example programs."           ${PROJECT_IS_TOP_LEVEL})
option(FOUNDRY_BUILD_WARNINGS      "Enable strict compiler warning suites."      ${PROJECT_IS_TOP_LEVEL})
option(FOUNDRY_WARNINGS_AS_ERRORS  "Treat compiler warnings as hard errors."     OFF)
option(FOUNDRY_CONSUMER_TESTS      "Build Foundry's consumer integration tests." OFF)

# ------------------------------------------------------------------------------
#  Option Validation & Derived Settings
# ------------------------------------------------------------------------------

if (DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain file: \"${CMAKE_TOOLCHAIN_FILE}\"")
endif()

if(FOUNDRY_WARNINGS_AS_ERRORS AND NOT FOUNDRY_BUILD_WARNINGS)
  message(WARNING
    "[Foundry] FOUNDRY_WARNINGS_AS_ERRORS requires FOUNDRY_BUILD_WARNINGS. "
    "Enabling FOUNDRY_BUILD_WARNINGS automatically."
  )
  set(FOUNDRY_BUILD_WARNINGS ON CACHE BOOL "" FORCE)
endif()

# ------------------------------------------------------------------------------
#  Global Settings
# ------------------------------------------------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Standard install directories (bin, lib, include, etc.) with platform-specific defaults.
include(GNUInstallDirs)

# ------------------------------------------------------------------------------
#  Output Layout & Platform Defaults  (top-level only)
# ------------------------------------------------------------------------------

if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

  # Single-config generators: default to Release when no build type is set.
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build configuration." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()
endif()

# ------------------------------------------------------------------------------
#  Generated Configuration / Versioning Headers
# ------------------------------------------------------------------------------
configure_file(
  "${FOUNDRY_BASE_FOLDER}/include/foundry/version.h.in"
  "${PROJECT_BINARY_DIR}/include/foundry/version.h"
  @ONLY
)

# ------------------------------------------------------------------------------
#  Library Sources Processing
# ------------------------------------------------------------------------------
add_library(Foundry INTERFACE)
add_library(Foundry::Foundry ALIAS Foundry)

target_include_directories(Foundry INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(Foundry INTERFACE cxx_std_17)

# ------------------------------------------------------------------------------
#  Install
# ------------------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
  include(foundry_install)
endif()

# ------------------------------------------------------------------------------
#  Examples and Tests
# ------------------------------------------------------------------------------

if(FOUNDRY_BUILD_EXAMPLES)
  message(STATUS "Foundry: Example targets are enabled.")
  add_subdirectory(examples)
endif()

if(FOUNDRY_BUILD_TESTS OR FOUNDRY_CONSUMER_TESTS)
  message(STATUS "Foundry: Test suite execution is enabled.")
  enable_testing()
  add_subdirectory(tests)
endif()
