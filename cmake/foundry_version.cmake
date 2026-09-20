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
# foundry_load_version(out_version out_core_version out_prerelease)
#
# Reads ${CMAKE_CURRENT_SOURCE_DIR}/VERSION and sets:
#   out_version       -> full raw version string, e.g. "1.2.3-WIP"
#   out_core_version  -> numeric-only string for project(), e.g. "1.2.3"
#   out_prerelease    -> prerelease identifier, e.g. "WIP"
#
# The configure dependency is registered so CMake re-runs automatically
# whenever the VERSION file changes.
# ------------------------------------------------------------------------------
function(foundry_load_version out_version out_core_version out_prerelease)
  set(_version_file "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

  if(NOT EXISTS "${_version_file}")
    message(FATAL_ERROR "[Foundry] VERSION file not found: ${_version_file}")
  endif()

  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_version_file}")

  file(READ "${_version_file}" _version)
  string(STRIP "${_version}" _version)

  # Validate the version format before stripping the prerelease suffix.
  if(NOT _version MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-WIP)?$")
    message(FATAL_ERROR
      "[Foundry] Invalid version '${_version}'. "
      "Expected format 'X.Y.Z' or 'X.Y.Z-WIP'."
    )
  endif()

  set(_core_version "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")

  if(_version MATCHES "-WIP$")
    set(_prerelease "WIP")
  else()
    set(_prerelease "")
  endif()

  set(${out_version}      "${_version}"      PARENT_SCOPE)
  set(${out_core_version} "${_core_version}" PARENT_SCOPE)
  set(${out_prerelease}   "${_prerelease}"   PARENT_SCOPE)
endfunction()
