# ==============================================================================
#  FOUNDRY - INSTALL
# ==============================================================================
#  Description: Defines install rules, CMake package config generation,
#               and the export set for downstream consumers.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

include_guard()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# ------------------------------------------------------------------------------
# Headers
# ------------------------------------------------------------------------------

install(
  DIRECTORY   "${PROJECT_SOURCE_DIR}/include/foundry"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Generated version header (produced by configure_file in the root CMakeLists).
install(
  FILES       "${PROJECT_BINARY_DIR}/include/foundry/version.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/foundry"
)

# ------------------------------------------------------------------------------
# Library target & export set
# ------------------------------------------------------------------------------

install(
  TARGETS     Foundry
  EXPORT      FoundryTargets
  INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
  EXPORT      FoundryTargets
  FILE        FoundryTargets.cmake
  NAMESPACE   Foundry::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Foundry"
)

# ------------------------------------------------------------------------------
# CMake package config
# ------------------------------------------------------------------------------

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/FoundryConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/cmake/FoundryConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Foundry"
)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/cmake/FoundryConfigVersion.cmake"
  VERSION       ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    "${PROJECT_BINARY_DIR}/cmake/FoundryConfig.cmake"
    "${PROJECT_BINARY_DIR}/cmake/FoundryConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Foundry"
)
