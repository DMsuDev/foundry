// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include <type_traits>

/// @file flags_traits.h
/// @brief Internal opt-in trait for foundry::types::Flags<E>.

namespace foundry::types::detail {

/**
 * @brief Opt-in trait that enables Flags<E> for a given enum type.
 *
 * Specialize via FOUNDRY_DECLARE_FLAGS(E) at namespace scope.
 * The default is std::false_type, which prevents accidental instantiation
 * of Flags<E> on arbitrary enums not designed as bitmasks.
 *
 * @tparam E Enum type to query.
 */
template <typename E>
struct enable_flags : std::false_type {};

/// @brief Convenience variable template.
template <typename E>
inline constexpr bool enable_flags_v = enable_flags<E>::value;

} // namespace foundry::types::detail
