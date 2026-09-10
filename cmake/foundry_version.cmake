# ==============================================================================
#  FOUNDRY - VERSION
# ==============================================================================
#  Description: Reads and parses the VERSION file, separating the plain
#               numeric version from any pre-release suffix (WIP).
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

include_guard()

# ------------------------------------------------------------------------------
# foundry_load_version(out_version out_plain_version)
#
# Reads ${CMAKE_CURRENT_SOURCE_DIR}/VERSION and sets:
#   out_version        -> full raw version string, e.g. "1.2.3-WIP"
#   out_plain_version  -> numeric-only string for project(), e.g. "1.2.3"
#
# The configure dependency is registered so CMake re-runs automatically
# whenever the VERSION file changes.
# ------------------------------------------------------------------------------
function(foundry_load_version out_version out_plain_version)
  set(_file "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

  if(NOT EXISTS "${_file}")
      message(FATAL_ERROR "[Foundry] VERSION file not found: ${_file}")
  endif()

  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_file}")

  file(READ "${_file}" _raw)
  string(STRIP "${_raw}" _raw)

  # Validate the version format before stripping the prerelease suffix.
  if(NOT _raw MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+(-WIP)?$")
    message(FATAL_ERROR
      "Invalid version '${_raw}'. "
      "Expected format 'X.Y.Z' or 'X.Y.Z-WIP'."
    )
  endif()

  string(REGEX REPLACE "-WIP$" "" _plain "${_raw}")

  set(${out_version}       "${_raw}"   PARENT_SCOPE)
  set(${out_plain_version} "${_plain}" PARENT_SCOPE)

  if(_raw MATCHES "-WIP$")
    set(FOUNDRY_IS_PRERELEASE TRUE PARENT_SCOPE)
  endif()
endfunction()
