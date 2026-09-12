// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/memory/run_once.h"

#include <iostream>

// ============================================================================
// Demos
// ============================================================================

static void Demo_RunOnce_Basic();
static void Demo_RunOnce_InsideLoop();
static void Demo_RunOnce_MultipleCallSites();
static void Demo_RunAtExit();

// ============================================================================
// Entry Point
// ============================================================================

int main()
{
    std::cout << "==========================================\n";
    std::cout << "      Foundry RunOnce / RunAtExit          \n";
    std::cout << "==========================================\n";

    Demo_RunOnce_Basic();
    Demo_RunOnce_InsideLoop();
    Demo_RunOnce_MultipleCallSites();
    Demo_RunAtExit();

    std::cout << "\n[main] Returning. RunAtExit blocks will fire after this.\n";

    return 0;
}

// ============================================================================
// Demo 1: Basic one-shot execution
// ============================================================================

// Demonstrates that FOUNDRY_RUN_ONCE only executes its block
// the first time the call site is reached, regardless of how
// many times the enclosing function is called.
static void Demo_RunOnce_Basic()
{
    std::cout << "\n--- Demo 1: Basic One-Shot Execution ---\n";

    for (int i = 1; i <= 3; ++i)
    {
        std::cout << "[" << i << "] Entering function...\n";

        FOUNDRY_RUN_ONCE {
            std::cout << "    - [RUN_ONCE] Initialization block executed.\n";
        };

        std::cout << "    - Regular work done.\n";
    }

    std::cout << "    Expected: initialization block printed exactly once.\n";
}

// ============================================================================
// Demo 2: Inside a loop with multiple guards
// ============================================================================

// Demonstrates that each FOUNDRY_RUN_ONCE macro expansion is its own
// independent one-shot guard. Two call sites in the same loop body
// each fire exactly once, independently.
static void Demo_RunOnce_InsideLoop()
{
    std::cout << "\n--- Demo 2: Multiple Guards Inside a Loop ---\n";

    for (int i = 1; i <= 4; ++i)
    {
        std::cout << "[" << i << "] Loop iteration.\n";

        FOUNDRY_RUN_ONCE {
            std::cout << "    - [GUARD A] Fired on first iteration only.\n";
        };

        if (i >= 2)
        {
            FOUNDRY_RUN_ONCE {
                std::cout << "    - [GUARD B] Fired the first time i >= 2.\n";
            };
        }
    }
}

// ============================================================================
// Demo 3: Simulated subsystem registration
// ============================================================================

// Demonstrates a realistic use case: registering subsystems or factories
// that must only run once even if the setup function is called multiple times.
static void register_subsystems()
{
    FOUNDRY_RUN_ONCE {
        std::cout << "    - Registering PhysicsComponent...\n";
        std::cout << "    - Registering RenderComponent...\n";
        std::cout << "    - Registering AudioComponent...\n";
        std::cout << "    - [RUN_ONCE] Subsystem registration complete.\n";
    };
}

static void Demo_RunOnce_MultipleCallSites()
{
    std::cout << "\n--- Demo 3: Subsystem Registration (called 3x) ---\n";

    register_subsystems();  // Registers
    register_subsystems();  // Skipped
    register_subsystems();  // Skipped

    std::cout << "    Expected: registration block printed exactly once.\n";
}

// ============================================================================
// Demo 4: RunAtExit - Fires at program shutdown
// ============================================================================

// Demonstrates FOUNDRY_RUN_AT_EXIT. The block is registered here but
// will not execute until the program exits (static storage destruction).
// Multiple registrations fire in reverse order of declaration (LIFO).
static void Demo_RunAtExit()
{
    std::cout << "\n--- Demo 4: RunAtExit (fires after main returns) ---\n";

    // Capture by value [=] only; locals won't exist at shutdown time.
    FOUNDRY_RUN_AT_EXIT {
        std::cout << "[RunAtExit] Flushing log system...\n";
    };

    FOUNDRY_RUN_AT_EXIT {
        std::cout << "[RunAtExit] Releasing global GPU resources...\n";
    };

    std::cout << "    RunAtExit blocks registered. They will fire after main() returns.\n";
    std::cout << "    Expected order (LIFO): GPU resources -> Log flush.\n";
}
