# ==============================================================================
#  FOUNDRY - EXAMPLE PROGRAMS
# ==============================================================================
#  Description: CMake configuration for building Foundry example programs.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

# ------------------------------------------------------------------------------
#  Example Registration Helper Function
# ------------------------------------------------------------------------------

function(foundry_add_example target_name source_file category)
  set(abs_source_file "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}")

  if(NOT EXISTS "${abs_source_file}")
    message(FATAL_ERROR "[Foundry] Example source file '${abs_source_file}' does not exist.")
  endif()

  add_executable("${target_name}" "${abs_source_file}")

  target_link_libraries("${target_name}" PRIVATE Foundry::Foundry)

  foundry_configure_compiler("${target_name}")
  foundry_configure_warnings("${target_name}")

  set_target_properties("${target_name}" PROPERTIES
    FOLDER "Examples/${category}"
  )
endfunction()

# ------------------------------------------------------------------------------
#  Example Target Declarations
# ------------------------------------------------------------------------------

# Memory Examples
foundry_add_example(scope_guard_example "memory/scope_guard_example.cpp" "memory")
foundry_add_example(run_once_example    "memory/run_once_example.cpp"    "memory")

# Type Examples
foundry_add_example(flags_example  "types/flags_example.cpp"  "types")
foundry_add_example(semver_example "types/semver_example.cpp" "types")
foundry_add_example(result_example "types/result_example.cpp" "types")

# Platform Examples
foundry_add_example(path_example "platform/paths_example.cpp" "platform")
