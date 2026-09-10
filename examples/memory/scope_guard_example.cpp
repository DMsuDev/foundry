// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/memory/scope_guard.h"

#include <iostream>
#include <fstream>
#include <string_view>

// ============================================================================
// Demos
// ============================================================================

static void Demo_UnconditionalCleanup();
static void Demo_RollbackOnFailure(bool simulate_failure);
static void Demo_FactoryUsage();

// ============================================================================
// Entry Point
// ============================================================================

int main()
{
    std::cout << "==========================================\n";
    std::cout << "       Foundry ScopeGuard Examples          \n";
    std::cout << "==========================================\n";

    Demo_UnconditionalCleanup();

    // Demonstrate the Rollback pattern
    Demo_RollbackOnFailure(true);   // Error path
    Demo_RollbackOnFailure(false);  // Success path

    Demo_FactoryUsage();

    return 0;
}

// ============================================================================
// Demo 1: Unconditional Cleanup
// ============================================================================

// Demonstrates FOUNDRY_ON_SCOPE_EXIT for releasing resources that MUST
// be cleaned up when exiting the block, regardless of how it finishes.
static void Demo_UnconditionalCleanup()
{
    std::cout << "\n--- Demo 1: Unconditional Cleanup ---\n";

    {
        std::cout << "[1] Allocating resource / opening block...\n";

        // This lambda will execute automatically upon leaving this {} block
        FOUNDRY_ON_SCOPE_EXIT({
            std::cout << "[3] [Cleanup] Resource released automatically via RAII.\n";
        });

        std::cout << "[2] Performing work inside the block...\n";
    } // <-- ScopeGuard is destroyed here and executes the cleanup block.

    std::cout << "[4] Block exited safely.\n";
}

// ============================================================================
// Demo 2: Rollback Pattern using dismiss()
// ============================================================================

// Demonstrates FOUNDRY_ON_SCOPE_EXIT_NAMED to disarm the guard if the
// operation completes successfully.
static void Demo_RollbackOnFailure(bool simulate_failure)
{
    std::cout << "\n--- Demo 2: Rollback Pattern (Simulate Failure: "
              << (simulate_failure ? "true" : "false") << ") ---\n";

    std::cout << "[1] Starting transactional operation...\n";
    std::cout << "    - Creating temporary database entry...\n";

    // Register the cancellation (Rollback) action by default
    FOUNDRY_ON_SCOPE_EXIT_NAMED(rollback_guard, {
        std::cout << "    - [ROLLBACK] Reverting database changes due to failure!\n";
    });

    std::cout << "    - Processing step 1: Success.\n";

    if (simulate_failure)
    {
        std::cout << "    - Processing step 2: FAILED! Exiting function early...\n";
        return; // Early return -> rollback_guard is NOT dismissed and triggers Rollback.
    }

    std::cout << "    - Processing step 2: Success.\n";

    // SUCCESS: Disarm the rollback action
    rollback_guard.dismiss();
    std::cout << "    - Operation SUCCESSFUL! Rollback dismissed via dismiss().\n";
}

// ============================================================================
// Demo 3: Factory Usage (make_scope_guard)
// ============================================================================

// Demonstrates creating a named guard directly using generic C++,
// useful when moving the guard or managing it outside macro abstraction.
static void Demo_FactoryUsage()
{
    std::cout << "\n--- Demo 3: Factory Usage (make_scope_guard) ---\n";

    auto cleanup = foundry::memory::make_scope_guard([]() {
        std::cout << "    - [Custom Guard] Explicitly created guard executed.\n";
    });

    std::cout << "    - Guard created with make_scope_guard().\n";
    std::cout << "    - Guard status: is_dismissed = "
              << (cleanup.is_dismissed() ? "true" : "false") << "\n";
}
