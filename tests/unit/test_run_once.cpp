// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/memory/run_once.h"

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

// ============================================================================
// RunOnce
// ============================================================================

TEST(RunOnce, ExecutesExactlyOnce)
{
    int count = 0;

    auto invoke = [&] {
        FOUNDRY_RUN_ONCE { count++; };
    };

    invoke();
    invoke();
    invoke();

    EXPECT_EQ(count, 1);
}

TEST(RunOnce, ExecutesOnFirstCallNotBefore)
{
    int count = 0;

    // Block has not run yet — count stays 0
    EXPECT_EQ(count, 0);

    FOUNDRY_RUN_ONCE { count = 42; };

    EXPECT_EQ(count, 42);

    // Subsequent calls in the same scope have no effect
    FOUNDRY_RUN_ONCE { count = 99; };  // different static, same value: fires once too
    // Note: this is a *different* call site so it also fires once here
    EXPECT_EQ(count, 99);
}

TEST(RunOnce, TwoIndependentCallSitesAreIndependent)
{
    int a = 0;
    int b = 0;

    auto invoke = [&] {
        FOUNDRY_RUN_ONCE { a++; };
        FOUNDRY_RUN_ONCE { b++; };
    };

    invoke();
    invoke();
    invoke();

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
}

TEST(RunOnce, InsideConditionalBranch)
{
    int count = 0;

    auto invoke = [&](bool condition) {
        if (condition) {
            FOUNDRY_RUN_ONCE { count++; };
        }
    };

    invoke(false);  // branch not taken — block not reached
    invoke(false);
    invoke(true);   // first time reached
    invoke(true);   // static already constructed, skip
    invoke(false);

    EXPECT_EQ(count, 1);
}

TEST(RunOnce, ThreadSafe)
{
    // C++11 guarantees static local initialization is atomic.
    // This test verifies no double-execution under concurrent access.
    std::atomic<int> count{ 0 };

    auto task = [&] {
        FOUNDRY_RUN_ONCE { count.fetch_add(1, std::memory_order_relaxed); };
    };

    constexpr int kThreads = 16;
    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int i = 0; i < kThreads; ++i)
        threads.emplace_back(task);

    for (auto& t : threads)
        t.join();

    EXPECT_EQ(count.load(), 1);
}

// ============================================================================
// RunAtExit
// ============================================================================

// RunAtExit fires at static destruction (program shutdown), so we cannot
// observe its execution within a test. What we *can* test is that the
// object constructs correctly, stores the callable, and that construction
// itself does not invoke the callable.

TEST(RunAtExit, DoesNotExecuteOnConstruction)
{
    int count = 0;

    // Constructing RunAtExit must NOT call the lambda immediately.
    {
        foundry::memory::RunAtExit guard{ [&] { count++; } };
        EXPECT_EQ(count, 0);

        // guard goes out of scope here: destructor fires (simulates shutdown
        // within this local scope for testability).
    }

    // After destruction, the lambda must have run exactly once.
    EXPECT_EQ(count, 1);
}

TEST(RunAtExit, ExecutesOnDestruction)
{
    int count = 0;

    {
        foundry::memory::RunAtExit guard{ [&] { count += 10; } };
        EXPECT_EQ(count, 0);
    }

    EXPECT_EQ(count, 10);
}

TEST(RunAtExit, MultipleGuardsDestructInLIFOOrder)
{
    std::vector<int> order;

    {
        foundry::memory::RunAtExit first { [&] { order.push_back(1); } };
        foundry::memory::RunAtExit second{ [&] { order.push_back(2); } };
        foundry::memory::RunAtExit third { [&] { order.push_back(3); } };
        // LIFO: third, second, first
    }

    ASSERT_EQ(order.size(), 3u);
    EXPECT_EQ(order[0], 3);
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 1);
}

TEST(RunAtExit, SwallowsExceptionsDuringDestruction)
{
    // Destructor is noexcept
    EXPECT_NO_THROW({
        foundry::memory::RunAtExit guard{ [] { throw std::runtime_error("shutdown error"); } };
    });
}

TEST(RunAtExit, MacroDoesNotExecuteImmediately)
{
    // The FOUNDRY_RUN_AT_EXIT macro creates a static; it should not fire
    // in the middle of this test body.
    bool fired = false;

    // We can't test the macro's static directly (it would fire at program
    // exit, not here), so we verify the underlying type instead.
    {
        foundry::memory::RunAtExit guard{ [&] { fired = true; } };
        EXPECT_FALSE(fired);  // not yet
    }

    EXPECT_TRUE(fired);  // fired on local destruction (scope exit here)
}
