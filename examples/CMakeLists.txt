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

# Logging Examples
# foundry_add_example(name "dir/name.cpp" "folder")
