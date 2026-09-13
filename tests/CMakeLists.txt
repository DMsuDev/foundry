# ==============================================================================
#  TEST SUITE ORCHESTRATOR
# ==============================================================================
#  Orchestrates test execution across two tiers:
#    - Unit (unit/)         : Fast GoogleTest suite (FOUNDRY_BUILD_TESTS)
#    - Integration (cmake_*): CMake consumer tests  (FOUNDRY_CONSUMER_TESTS)
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

if(FOUNDRY_BUILD_TESTS)
  add_subdirectory(unit)
endif()

# ------------------------------------------------------------------------------
#  Install / Consumer Integration Tests (opt-in, slow)
# ------------------------------------------------------------------------------

if(FOUNDRY_CONSUMER_TESTS)
  message(STATUS "Foundry: Registering install/consumer integration tests.")

  add_subdirectory(cmake_fetch_content)
  add_subdirectory(cmake_add_subdirectory)
endif()
