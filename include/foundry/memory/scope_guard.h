// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include "foundry/detail/preprocessor.h"

#include <type_traits>
#include <utility>

/// @file scope_guard.h
/// @brief Scope guard (RAII) and macros for automatic resource cleanup.

namespace foundry::memory {

// ============================================================================
// ScopeGuard<F>
// ============================================================================

/**
 * @brief RAII guard that executes a callable on scope exit unless dismissed.
 *
 * @tparam F Callable type with signature void().
 *
 * @note Exceptions inside the callback are caught and swallowed in the destructor
 *       to prevent std::terminate during stack unwinding.
 */
template <typename F>
class ScopeGuard {
public:
    static_assert(
        std::is_invocable_v<F>,
        "foundry::ScopeGuard requires an invocable type with signature void()"
    );

    /// @brief Constructs a guard that will invoke @p f on scope exit.
    explicit ScopeGuard(F&& f)
        : m_f(std::forward<F>(f))
        , m_dismissed(false)
    {}

    /// @brief Move constructor. Transfers ownership and disarms the moved-from guard.
    ScopeGuard(ScopeGuard&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
        : m_f(std::move(other.m_f))
        , m_dismissed(other.m_dismissed)
    {
        // Disarm the moved-from instance to prevent double execution.
        other.m_dismissed = true;
    }

    ScopeGuard(const ScopeGuard&)            = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&)      = delete;

    ~ScopeGuard() noexcept {
        if (!m_dismissed) {
            try {
                m_f();
            } catch (...) {
                // Swallowed: destruction must never throw.
            }
        }
    }

    /// @brief Cancels the callback. Safe to call multiple times.
    void dismiss() noexcept { m_dismissed = true; }

    /// @brief Checks whether the cleanup action has been cancelled.
    [[nodiscard]] bool is_dismissed() const noexcept { return m_dismissed; }

private:
    F    m_f;
    bool m_dismissed;
};

/**
 * @brief Helper factory to construct a ScopeGuard without specifying type template arguments.
 *
 * @param f Callable to execute on scope exit.
 *
 * @warning Do not ignore the return value; doing so destroys the guard immediately.
 */
template <typename F>
[[nodiscard]] ScopeGuard<std::decay_t<F>> make_scope_guard(F&& f) {
    return ScopeGuard<std::decay_t<F>>(std::forward<F>(f));
}

} // namespace foundry::memory

// ============================================================================
// Macro-based API
// ============================================================================

/**
 * @def FOUNDRY_SCOPE_EXIT(code)
 * @brief Executes @p code automatically when exiting the current scope.
 *
 * Example:
 * @code{.cpp}
 * FOUNDRY_SCOPE_EXIT(fclose(file));
 * @endcode
 */
#define FOUNDRY_SCOPE_EXIT(code) \
    auto FOUNDRY_DETAIL_UNIQUE_NAME(foundry_scope_guard_) = \
        ::foundry::memory::make_scope_guard([&]() noexcept { code; })

/**
 * @def FOUNDRY_SCOPE_EXIT_NAMED(name, code)
 * @brief Creates a named scope guard that can be cancelled with @c name.dismiss().
 *
 * Example:
 * @code{.cpp}
 * FOUNDRY_SCOPE_EXIT_NAMED(guard, db.rollback());
 * // ... perform operations ...
 * guard.dismiss(); // Operation succeeded, skip rollback
 * @endcode
 */
#define FOUNDRY_SCOPE_EXIT_NAMED(name, code) \
    auto name = ::foundry::memory::make_scope_guard([&]() noexcept { code; })
