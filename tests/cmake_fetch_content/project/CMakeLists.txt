# ==============================================================================
#  FOUNDRY - INSTALL INTEGRATION TEST: cmake_fetch_content (consumer project)
# ==============================================================================
#  Description: Standalone consumer project simulating an external user who
#               pulls in Foundry via FetchContent. A local SOURCE_DIR is used
#               instead of GIT_REPOSITORY so the test runs offline against the
#               current source tree.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

cmake_minimum_required(VERSION 3.28)

project(foundry_smoke_fetch_content LANGUAGES CXX)

if(NOT DEFINED FOUNDRY_SOURCE_DIR)
  message(FATAL_ERROR "FOUNDRY_SOURCE_DIR must be provided (path to the Foundry source root).")
endif()

include(FetchContent)

FetchContent_Declare(
  foundry
  SOURCE_DIR "${FOUNDRY_SOURCE_DIR}"
)
FetchContent_MakeAvailable(foundry)

add_executable(foundry_smoke main.cpp)

target_link_libraries(foundry_smoke PRIVATE Foundry::Foundry)

target_compile_features(foundry_smoke PRIVATE cxx_std_17)

# Pin the output path regardless of single- vs multi-config generator so the
# harness in ../CMakeLists.txt can locate the binary with a fixed --test-command.
set(foundry_smoke_output_dir "${CMAKE_CURRENT_BINARY_DIR}/bin")
set_target_properties(foundry_smoke PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY                 "${foundry_smoke_output_dir}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG           "${foundry_smoke_output_dir}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE         "${foundry_smoke_output_dir}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL      "${foundry_smoke_output_dir}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${foundry_smoke_output_dir}"
)
