// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include "foundry/detail/visibility.h"
#include <filesystem>

namespace foundry::platform {

/// @brief Returns the process working directory.
/// @return The current working directory, or an empty path if it cannot be determined.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path working_directory();

/// @brief Returns the absolute path of the running executable.
/// @throws std::runtime_error if the executable path cannot be retrieved.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path executable_path();

/// @brief Returns the directory containing the running executable.
/// @throws std::runtime_error if the executable directory cannot be determined.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path executable_directory();

/// @brief Returns the system temporary directory.
/// @return The temporary directory path, or an empty path if it cannot be determined.
[[nodiscard]]
FOUNDRY_EXPORT std::filesystem::path temp_directory();

} // namespace foundry::platform
