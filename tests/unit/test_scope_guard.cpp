// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/memory/scope_guard.h"

#include <gtest/gtest.h>
#include <stdexcept>

// ============================================================================
// Basic Execution Tests
// ============================================================================

TEST(ScopeGuardTest, ExecutesOnScopeExit)
{
    bool executed = false;

    {
        auto guard = foundry::memory::make_scope_guard([&]() {
            executed = true;
        });
        EXPECT_FALSE(executed);
    } // Guard goes out of scope here

    EXPECT_TRUE(executed);
}

TEST(ScopeGuardTest, DismissPreventsExecution)
{
    bool executed = false;

    {
        auto guard = foundry::memory::make_scope_guard([&]() {
            executed = true;
        });

        EXPECT_FALSE(guard.is_dismissed());
        guard.dismiss();
        EXPECT_TRUE(guard.is_dismissed());
    }

    EXPECT_FALSE(executed);
}

// ============================================================================
// Move Semantics & Ownership Transfer Tests
// ============================================================================

TEST(ScopeGuardTest, MoveConstructorTransfersOwnership)
{
    int execution_count = 0;

    {
        auto original_guard = foundry::memory::make_scope_guard([&]() {
            execution_count++;
        });

        // Move ownership to moved_guard
        auto moved_guard = std::move(original_guard);

        // Original guard should now be disarmed
        EXPECT_TRUE(original_guard.is_dismissed());
        EXPECT_FALSE(moved_guard.is_dismissed());
    }

    // Callback should execute exactly ONCE (from moved_guard)
    EXPECT_EQ(execution_count, 1);
}

TEST(ScopeGuardTest, MoveAssignmentTransfersOwnership)
{
    int count = 0;

    {
        auto guard1 = foundry::memory::make_scope_guard([&]() { count++; });

        // Assigning guard1 into guard2 transfers ownership
        auto guard2 = std::move(guard1);

        EXPECT_TRUE(guard1.is_dismissed());
        EXPECT_FALSE(guard2.is_dismissed());
    }

    // count should be 1 (executed via guard2).
    EXPECT_EQ(count, 1);
}

// ============================================================================
// Exception Handling & Safety Tests
// ============================================================================

TEST(ScopeGuardTest, ExecutesDuringStackUnwinding)
{
    bool executed = false;

    try
    {
        auto guard = foundry::memory::make_scope_guard([&]() {
            executed = true;
        });

        throw std::runtime_error("Simulated exception");
    }
    catch (const std::runtime_error&)
    {
        // Exception caught, scope guard should have executed during unwinding
    }

    EXPECT_TRUE(executed);
}

TEST(ScopeGuardTest, AbsorbsExceptionsInCallback)
{
    // Verifies that noexcept destructor catches exceptions thrown inside m_f()
    // without invoking std::terminate().
    EXPECT_NO_THROW({
        auto guard = foundry::memory::make_scope_guard([]() {
            throw std::runtime_error("Exception inside cleanup callback");
        });
    });
}

// ============================================================================
// Macro Integration Tests
// ============================================================================

TEST(ScopeGuardTest, MacroOnScopeExit)
{
    bool executed = false;

    {
        FOUNDRY_SCOPE_EXIT(
            executed = true;
        );
        EXPECT_FALSE(executed);
    }

    EXPECT_TRUE(executed);
}

TEST(ScopeGuardTest, MacroOnScopeExitNamed)
{
    bool executed = false;

    {
        FOUNDRY_SCOPE_EXIT_NAMED(rollback, {
            executed = true;
        });

        rollback.dismiss();
    }

    EXPECT_FALSE(executed);
}
