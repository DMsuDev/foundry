// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include "foundry/types/detail/flags_traits.h"
#include "foundry/detail/preprocessor.h"

#include <type_traits>

/// @file flags.h
/// @brief Type-safe bitmask wrapper over scoped enums.

namespace foundry::types {

// ============================================================================
// Flags<E>
// ============================================================================

/**
 * @brief Type-safe bitmask built on top of a scoped enum.
 *
 * Wraps the underlying integer representation of an `enum class` and exposes
 * bitwise operations while preserving type safety. Direct integer arithmetic
 * on `E` is intentionally not supported; all manipulation goes through
 * `Flags<E>`.
 *
 * The enum must be opted-in via @ref FOUNDRY_DECLARE_FLAGS before use. All
 * query and mutation methods accept both `E` (single enumerator) and `Flags<E>`
 * (combined mask), so callers never need to construct a `Flags<E>` explicitly
 * just to test or set a single bit.
 *
 * @warning `operator~` inverts the full underlying integer, including bits with
 *          no corresponding enumerator. Prefer explicit @ref unset() in
 *          high-level code where only defined bits should be affected.
 *
 * @tparam E A scoped enum type opted-in via @ref FOUNDRY_DECLARE_FLAGS.
 *
 * @see FOUNDRY_DECLARE_FLAGS
 * @see FOUNDRY_BIT
 * @see FOUNDRY_BIT64
 */
template <typename E>
class Flags {
    static_assert(
        std::is_enum_v<E>,
        "foundry::Flags requires an enum type."
    );
    static_assert(
        ::foundry::types::detail::enable_flags_v<E>,
        "foundry::Flags<E>: E must be declared with FOUNDRY_DECLARE_FLAGS(E)."
    );

public:
    /// @brief Underlying integer type of the enum.
    using underlying_type = std::underlying_type_t<E>;

    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------

    /// @brief Constructs an empty (zero) flag set.
    constexpr Flags() noexcept : m_bits(0) {}

    /// @brief Constructs a flag set from a single enumerator.
    constexpr Flags(E value) noexcept
        : m_bits(static_cast<underlying_type>(value))
    {}

    // -------------------------------------------------------------------------
    // Bitwise assignment operators
    // -------------------------------------------------------------------------

    constexpr Flags& operator|=(Flags other) noexcept { m_bits |= other.m_bits; return *this; }
    constexpr Flags& operator&=(Flags other) noexcept { m_bits &= other.m_bits; return *this; }
    constexpr Flags& operator^=(Flags other) noexcept { m_bits ^= other.m_bits; return *this; }

    // -------------------------------------------------------------------------
    // Bitwise operators
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr Flags operator|(Flags other) const noexcept { return Flags(m_bits | other.m_bits); }
    [[nodiscard]] constexpr Flags operator&(Flags other) const noexcept { return Flags(m_bits & other.m_bits); }
    [[nodiscard]] constexpr Flags operator^(Flags other) const noexcept { return Flags(m_bits ^ other.m_bits); }

    /// @brief Returns the bitwise complement of this flag set.
    /// @warning Inverts all bits in the underlying integer, including those
    ///          with no corresponding enumerator. See class-level warning.
    [[nodiscard]] constexpr Flags operator~() const noexcept { return Flags(~m_bits); }

    // -------------------------------------------------------------------------
    // Comparison
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr bool operator==(Flags other) const noexcept { return m_bits == other.m_bits; }
    [[nodiscard]] constexpr bool operator!=(Flags other) const noexcept { return m_bits != other.m_bits; }

    // -------------------------------------------------------------------------
    // Query
    // -------------------------------------------------------------------------

    /// @brief Returns true if ALL bits in @p flags are set.
    [[nodiscard]] constexpr bool has(E flag) const noexcept {
        return (m_bits & static_cast<underlying_type>(flag)) != 0;
    }

    /// @brief Returns true if ALL bits in @p flags are set.
    [[nodiscard]] constexpr bool has(Flags flags) const noexcept {
        return (m_bits & flags.m_bits) == flags.m_bits;
    }

    /// @brief Returns true if ANY bit is set.
    [[nodiscard]] constexpr bool any() const noexcept { return m_bits != 0; }

    /// @brief Returns true if NO bit is set.
    [[nodiscard]] constexpr bool none() const noexcept { return m_bits == 0; }

    // -------------------------------------------------------------------------
    // Mutation
    // -------------------------------------------------------------------------

    /// @brief Sets the single enumerator @p flag.
    constexpr void set(E flag) noexcept    { m_bits |=  static_cast<underlying_type>(flag); }

    /// @brief Sets all bits in @p flags.
    constexpr void set(Flags flags) noexcept   { m_bits |=  flags.m_bits; }

    /// @brief Clears the single enumerator @p flag.
    constexpr void unset(E flag) noexcept  { m_bits &= ~static_cast<underlying_type>(flag); }

    /// @brief Clears all bits in @p flags.
    constexpr void unset(Flags flags) noexcept { m_bits &= ~flags.m_bits; }

    /// @brief Toggles the single enumerator @p flag.
    constexpr void toggle(E flag) noexcept  { m_bits ^=  static_cast<underlying_type>(flag); }

    /// @brief Toggles all bits in @p flags.
    constexpr void toggle(Flags flags) noexcept { m_bits ^= flags.m_bits; }

    /// @brief Clears all bits.
    constexpr void clear() noexcept { m_bits = 0; }

    // -------------------------------------------------------------------------
    // Raw access
    // -------------------------------------------------------------------------

    /// @brief Returns the raw underlying integer value.
    [[nodiscard]] constexpr underlying_type bits() const noexcept { return m_bits; }

    /// @brief Explicit conversion to bool — true if any bit is set.
    [[nodiscard]] explicit constexpr operator bool() const noexcept { return any(); }

private:
    /// @brief Private constructor from a raw integer, used by bitwise operators.
    explicit constexpr Flags(underlying_type bits) noexcept : m_bits(bits) {}

    underlying_type m_bits;
};

} // namespace foundry::types

// ============================================================================
// Free-function bitwise operators
// ============================================================================
//
// Enable bitwise operations between opted-in enums and Flags<E>, producing
// Flags<E> without requiring explicit construction.

/// @cond INTERNAL

template <typename E, typename = std::enable_if_t<::foundry::types::detail::enable_flags_v<E>>>
[[nodiscard]] constexpr foundry::types::Flags<E> operator|(E lhs, E rhs) noexcept {
    return foundry::types::Flags<E>(lhs) | foundry::types::Flags<E>(rhs);
}

template <typename E, typename = std::enable_if_t<::foundry::types::detail::enable_flags_v<E>>>
[[nodiscard]] constexpr foundry::types::Flags<E> operator&(E lhs, E rhs) noexcept {
    return foundry::types::Flags<E>(lhs) & foundry::types::Flags<E>(rhs);
}

template <typename E, typename = std::enable_if_t<::foundry::types::detail::enable_flags_v<E>>>
[[nodiscard]] constexpr foundry::types::Flags<E> operator^(E lhs, E rhs) noexcept {
    return foundry::types::Flags<E>(lhs) ^ foundry::types::Flags<E>(rhs);
}

template <typename E, typename = std::enable_if_t<::foundry::types::detail::enable_flags_v<E>>>
[[nodiscard]] constexpr foundry::types::Flags<E> operator~(E value) noexcept {
    return ~foundry::types::Flags<E>(value);
}

template <typename E, typename = std::enable_if_t<::foundry::types::detail::enable_flags_v<E>>>
[[nodiscard]] constexpr foundry::types::Flags<E> operator|(E lhs, foundry::types::Flags<E> rhs) noexcept {
    return foundry::types::Flags<E>(lhs) | rhs;
}

template <typename E, typename = std::enable_if_t<::foundry::types::detail::enable_flags_v<E>>>
[[nodiscard]] constexpr foundry::types::Flags<E> operator&(E lhs, foundry::types::Flags<E> rhs) noexcept {
    return foundry::types::Flags<E>(lhs) & rhs;
}

/// @endcond

// ============================================================================
// FOUNDRY_DECLARE_FLAGS
// ============================================================================

/**
 * @def FOUNDRY_DECLARE_FLAGS(E)
 * @brief Opts a scoped enum into the @ref foundry::types::Flags system.
 *
 * Must be placed at namespace scope, outside the enum definition. Specializes
 * the internal @ref foundry::types::detail::enable_flags trait so that
 * `Flags<E>` compiles and all bitwise free functions are enabled via SFINAE.
 *
 * @param E Scoped enum type to enable.
 *
 * @see foundry::types::Flags
 * @see FOUNDRY_BIT
 * @see FOUNDRY_BIT64
 */
#define FOUNDRY_DECLARE_FLAGS(E)                                          \
    template <>                                                           \
    struct ::foundry::types::detail::enable_flags<E> : std::true_type {};
