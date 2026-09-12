// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include "foundry/detail/preprocessor.h"

#include <type_traits>
#include <utility>

/// @file run_once.h
/// @brief One-shot execution utilities: RunOnce (first call) and RunAtExit (program shutdown).

namespace foundry::memory {

// ============================================================================
// RunOnce
// ============================================================================

/**
 * @brief Executes a callable exactly once at the call site.
 *
 * The one-shot guarantee comes from the @c static storage of the variable
 * declared by @c FOUNDRY_RUN_ONCE; not from this class itself.
 *
 * Thread-safe: C++11 guarantees that static local initialization is atomic.
 *
 * @tparam F Callable type with signature void().
 *
 * @warning Do not use directly. Prefer @c FOUNDRY_RUN_ONCE.
 */
template <typename F>
class RunOnce {
public:
    static_assert(
        std::is_invocable_r_v<void, F>,
        "foundry::RunOnce<F>: F must be callable with signature void()"
    );

    /// @brief Executes @p f immediately upon construction.
    explicit RunOnce(F&& f) {
        std::forward<F>(f)();
    }

    RunOnce(const RunOnce&)            = delete;
    RunOnce(RunOnce&&)                 = delete;
    RunOnce& operator=(const RunOnce&) = delete;
    RunOnce& operator=(RunOnce&&)      = delete;
};

// ============================================================================
// RunAtExit
// ============================================================================

/**
 * @brief Executes a callable when the program exits (static storage destruction).
 *
 * Destruction order follows LIFO across translation units that share the same
 * static initialization order. Behaves like @c std::atexit but with lambda
 * syntax and no function-pointer limitation.
 *
 * @tparam F Callable type with signature void().
 *
 * @warning Capture by value (@c [=]) only. Variables captured by reference
 *          may have been destroyed by the time the destructor runs.
 *
 * @warning Do not use directly. Prefer @c FOUNDRY_RUN_AT_EXIT.
 */
template <typename F>
class RunAtExit {
public:
    static_assert(
        std::is_invocable_r_v<void, F>,
        "foundry::RunAtExit<F>: F must be callable with signature void()"
    );

    /// @brief Stores @p f; it will be invoked in the destructor.
    explicit RunAtExit(F&& f)
        : m_f(std::forward<F>(f))
    {}

    /// @brief Invokes the stored callable during program shutdown.
    ~RunAtExit() noexcept {
        try {
            m_f();
        } catch (...) {
            // Swallowed: destruction must never throw.
        }
    }

    RunAtExit(const RunAtExit&)            = delete;
    RunAtExit(RunAtExit&&)                 = delete;
    RunAtExit& operator=(const RunAtExit&) = delete;
    RunAtExit& operator=(RunAtExit&&)      = delete;

private:
    F m_f;
};

namespace detail {

    // --- RunOnce sentinel ----------------------------------------------------

    struct RunOnceOnEnter {};

    template <typename F>
    RunOnce<F> operator+(RunOnceOnEnter, F&& f) {
        return RunOnce<F>(std::forward<F>(f));
    }

    // --- RunAtExit sentinel ----------------------------------------------------

    struct RunAtExitOnEnter {};

    template <typename F>
    RunAtExit<std::decay_t<F>> operator+(RunAtExitOnEnter, F&& f) {
        return RunAtExit<std::decay_t<F>>(std::forward<F>(f));
    }

} // namespace foundry::memory::detail

} // namespace foundry::memory

// ============================================================================
// Macro-based API
// ============================================================================

#define FOUNDRY_DETAIL_RUN_ONCE \
    ::foundry::memory::detail::RunOnceOnEnter() + [&]()

#define FOUNDRY_DETAIL_RUN_AT_EXIT \
    ::foundry::memory::detail::RunAtExitOnEnter() + [=]()

/**
 * @def FOUNDRY_RUN_ONCE
 * @brief Executes a block only the first time this line is reached at runtime.
 *
 * @code{.cpp}
 * void init_subsystem() {
 *     FOUNDRY_RUN_ONCE {
 *         AudioSystem::global_init();
 *     };
 * }
 * @endcode
 */
#define FOUNDRY_RUN_ONCE \
    [[maybe_unused]] static auto FOUNDRY_DETAIL_UNIQUE_NAME(foundry_run_once_) = \
        FOUNDRY_DETAIL_RUN_ONCE

/**
 * @def FOUNDRY_RUN_AT_EXIT
 * @brief Executes a block when the program exits (static storage destruction).
 *
 * Equivalent to @c std::atexit but with lambda syntax. Destruction order
 * across translation units follows LIFO of static initialization.
 *
 * @warning Always capture by value inside the block. References to local
 *          variables will be dangling at shutdown time.
 *
 * @code{.cpp}
 * void setup_logging() {
 *     FOUNDRY_RUN_AT_EXIT {
 *         LogSystem::flush_all();  // capture by value only
 *     };
 * }
 * @endcode
 */
#define FOUNDRY_RUN_AT_EXIT \
    [[maybe_unused]] static auto FOUNDRY_DETAIL_UNIQUE_NAME(foundry_run_at_exit_) = \
        FOUNDRY_DETAIL_RUN_AT_EXIT
