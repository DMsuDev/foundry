// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

// ============================================================================
// Internal helpers
// ============================================================================

/// @cond INTERNAL

// Stringification: two levels required to expand macros before quoting.
#define FOUNDRY_DETAIL_STRINGIFY_IMPL(x) #x
#define FOUNDRY_DETAIL_STRINGIFY(x)      FOUNDRY_DETAIL_STRINGIFY_IMPL(x)

// Token concatenation: two levels required to expand macros before pasting.
#define FOUNDRY_DETAIL_CONCAT_IMPL(a, b) a##b
#define FOUNDRY_DETAIL_CONCAT(a, b)      FOUNDRY_DETAIL_CONCAT_IMPL(a, b)

// Unique name generation based on the current source line.
#define FOUNDRY_DETAIL_UNIQUE_NAME(prefix) FOUNDRY_DETAIL_CONCAT(prefix, __LINE__)

/// @endcond
