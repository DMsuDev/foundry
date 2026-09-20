# ==============================================================================
#  FOUNDRY - TEST SUITE
# ==============================================================================
#  Description: Build unit tests against GoogleTest.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

# ------------------------------------------------------------------------------
#  Dependency: GoogleTest
# ------------------------------------------------------------------------------
if(NOT TARGET GTest::gtest)
  include(FetchContent)

  # Match MSVC runtime settings to avoid mismatches on Windows
  set(gtest_force_shared_crt ON  CACHE BOOL "Use shared CRT for gtest" FORCE)

  # Disable building the gmock framework to reduce build footprint.
  set(BUILD_GMOCK OFF CACHE BOOL "Disable gmock framework" FORCE)

  # Disable installation rules for GoogleTest to avoid polluting system paths.
  set(INSTALL_GTEST OFF CACHE BOOL "Disable gtest installation rules" FORCE)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
    SYSTEM
  )
  FetchContent_MakeAvailable(googletest)
endif()

# Load the native GoogleTest discovery modules
include(GoogleTest)

# ------------------------------------------------------------------------------
#  Source File Maps
# ------------------------------------------------------------------------------
file(GLOB_RECURSE FOUNDRY_GTEST_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT FOUNDRY_GTEST_SOURCES)
  message(WARNING "[Foundry] No Foundry unit test sources were found.")
  return()
endif()

# ------------------------------------------------------------------------------
#  Test Binary Target Initialization
# ------------------------------------------------------------------------------
add_executable(foundry_tests ${FOUNDRY_GTEST_SOURCES})

target_link_libraries(foundry_tests
  PRIVATE
    Foundry::Foundry
    GTest::gtest
    GTest::gtest_main
)

target_include_directories(foundry_tests
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# ------------------------------------------------------------------------------
#  Target Policies & System Adjustments
# ------------------------------------------------------------------------------
foundry_configure_compiler(foundry_tests)
foundry_configure_warnings(foundry_tests)
foundry_enable_coverage(foundry_tests)

set_target_properties(foundry_tests PROPERTIES
  FOLDER "Foundry/Tests"
)

# ------------------------------------------------------------------------------
#  Automated Test Discovery Registry
# ------------------------------------------------------------------------------
gtest_discover_tests(foundry_tests
  PROPERTIES
    LABELS "foundry;unit"
  DISCOVERY_TIMEOUT 30
  DISCOVERY_MODE    PRE_TEST
)
