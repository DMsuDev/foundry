// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include "foundry/detail/visibility.h"

#include <filesystem>
#include <system_error>

/// @file paths.h
/// @brief Platform-independent path utilities.

namespace foundry::platform {

/// @brief Returns the current working directory.
///
/// @throws std::system_error if the directory cannot be retrieved.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path working_directory();

/// @brief Returns the current working directory as a path object.
///
/// @param ec Receives the error code if the operation fails.
/// @return The current working directory, or an empty path on failure.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path working_directory(std::error_code& ec) noexcept;

/// @brief Returns the absolute path to the running executable.
///
/// @throws std::system_error if the executable path cannot be retrieved.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path executable_path();

/// @brief Returns the absolute path to the running executable.
///
/// @param ec Receives the error code if the operation fails.
/// @return The executable path, or an empty path on failure.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path executable_path(std::error_code& ec) noexcept;

/// @brief Returns the directory containing the running executable.
///
/// @throws std::system_error if the executable path cannot be retrieved.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path executable_directory();

/// @brief Returns the directory containing the running executable.
///
/// @param ec Receives the error code if the operation fails.
/// @return The executable directory, or an empty path on failure.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path executable_directory(std::error_code& ec) noexcept;

/// @brief Returns the platform's temporary directory.
///
/// @throws std::system_error if the directory cannot be retrieved.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path temp_directory();

/// @brief Returns the platform's temporary directory.
///
/// @param ec Receives the error code if the operation fails.
/// @return The temporary directory, or an empty path on failure.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path temp_directory(std::error_code& ec) noexcept;

} // namespace foundry::platform
