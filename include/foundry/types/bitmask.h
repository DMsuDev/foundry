// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

/// @file bitmask.h
/// @brief Portable bit-position macros for defining bitmask enumerators.

// ============================================================================
// Bit macros
// ============================================================================

/**
 * @def FOUNDRY_BIT(n)
 * @brief Produces a 32-bit bitmask with bit @p n set.
 *
 * Equivalent to `(1u << n)`. The valid bit positions are 0 through 31.
 *
 * @param n Zero-based bit position.
 */
#define FOUNDRY_BIT(n) (1u << (n))

/**
 * @def FOUNDRY_BIT64(n)
 * @brief Produces a 64-bit bitmask with bit @p n set.
 *
 * Equivalent to `(1ull << n)`. The valid bit positions are 0 through 63.
 *
 * @param n Zero-based bit position.
 */
#define FOUNDRY_BIT64(n) (1ull << (n))
