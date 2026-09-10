// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/types/bitmask.h" // OPTIONAL: provides FOUNDRY_BIT macros
#include "foundry/types/flags.h"

#include <cstdint>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

// =============================================================================
// Example enum: file permissions
// =============================================================================
//
// FOUNDRY_BIT is an optional convenience macro for defining bit values.
// It is not required by the Flags system.
//
// The following are equivalent:
//
//     Read = FOUNDRY_BIT(0)
//     Read = 1u << 0
//
// You can therefore use standard C++ bit-shift expressions instead of the
// FOUNDRY_BIT macros if preferred.
//

enum class Permission : std::uint32_t {
    None    = 0,
    Read    = FOUNDRY_BIT(0), // Equivalent to: 1u << 0
    Write   = FOUNDRY_BIT(1), // Equivalent to: 1u << 1
    Execute = FOUNDRY_BIT(2), // Equivalent to: 1u << 2
};

FOUNDRY_DECLARE_FLAGS(Permission)

// =============================================================================
// Example enum: large flag set (64-bit)
// =============================================================================
//
// FOUNDRY_BIT64 is also optional. The same values can be expressed using
// standard C++:
//
//     DarkMode    = 1ull << 0
//     Telemetry   = 1ull << 1
//     BetaUpdates = 1ull << 2
//     DebugUI     = 1ull << 63
//

enum class FeatureFlag : std::uint64_t {
    None        = 0,
    DarkMode    = FOUNDRY_BIT64(0),
    Telemetry   = FOUNDRY_BIT64(1),
    BetaUpdates = FOUNDRY_BIT64(2),
    DebugUI     = FOUNDRY_BIT64(63),
};

FOUNDRY_DECLARE_FLAGS(FeatureFlag)

// =============================================================================
// Helpers
// =============================================================================

using foundry::types::Flags;

static void print_permissions(Flags<Permission> perms)
{
    std::cout << "  Permissions:\n";
    std::cout << "    Read:    " << (perms.has(Permission::Read)    ? "yes" : "no") << '\n';
    std::cout << "    Write:   " << (perms.has(Permission::Write)   ? "yes" : "no") << '\n';
    std::cout << "    Execute: " << (perms.has(Permission::Execute) ? "yes" : "no") << '\n';
    std::cout << "    any():   " << (perms.any()  ? "yes" : "no") << '\n';
    std::cout << "    none():  " << (perms.none() ? "yes" : "no") << '\n';
}

// =============================================================================
// Entry Point
// =============================================================================

int main()
{
    // Enable UTF-8 output for the console (if supported)
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "==========================================\n";
    std::cout << "          Foundry Flags Examples          \n";
    std::cout << "==========================================\n";

    // -------------------------------------------------------------------------
    // 1. Combining enumerators with operator|
    // -------------------------------------------------------------------------
    std::cout << "--- Demo 1: Combining enumerators ---\n";

    Flags<Permission> perms = Permission::Read | Permission::Write;
    print_permissions(perms);

    // -------------------------------------------------------------------------
    // 2. Testing individual bits with has(E)
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 2: has(E) — single enumerator ---\n";

    if (perms.has(Permission::Read)) {
        std::cout << "  Read is set.\n";
    }
    if (!perms.has(Permission::Execute)) {
        std::cout << "  Execute is not set.\n";
    }

    // -------------------------------------------------------------------------
    // 3. Testing a combined mask with has(Flags<E>)
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 3: has(Flags<E>) -> combined mask ---\n";

    Flags<Permission> readWrite = Permission::Read | Permission::Write;
    if (perms.has(readWrite)) {
        std::cout << "  Has both Read and Write.\n";
    }

    // -------------------------------------------------------------------------
    // 4. Mutating the flag set
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 4: set / unset / toggle ---\n";

    perms.set(Permission::Execute);
    std::cout << "  After set(Execute):\n";
    print_permissions(perms);

    perms.unset(Permission::Write);
    std::cout << "  After unset(Write):\n";
    print_permissions(perms);

    perms.toggle(Permission::Read);
    std::cout << "  After toggle(Read):\n";
    print_permissions(perms);

    // -------------------------------------------------------------------------
    // 5. Clearing all bits
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 5: clear() ---\n";

    perms.clear();
    std::cout << "  After clear():\n";
    print_permissions(perms);

    // -------------------------------------------------------------------------
    // 6. Explicit bool conversion
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 6: operator bool ---\n";

    Flags<Permission> empty;
    Flags<Permission> nonEmpty = Permission::Read;

    std::cout << "     empty -> " << (empty    ? "true" : "false") << '\n';
    std::cout << "  nonEmpty -> " << (nonEmpty ? "true" : "false") << '\n';

    // -------------------------------------------------------------------------
    // 7. Equality comparison
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 7: operator== / operator!= ---\n";

    Flags<Permission> a = Permission::Read | Permission::Write;
    Flags<Permission> b = Permission::Read | Permission::Write;
    Flags<Permission> c = Permission::Execute;

    std::cout << "  a == b: " << (a == b ? "true" : "false") << '\n';
    std::cout << "  a != c: " << (a != c ? "true" : "false") << '\n';

    // -------------------------------------------------------------------------
    // 8. 64-bit flags
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 8: 64-bit flags ---\n";

    Flags<FeatureFlag> features = FeatureFlag::DarkMode | FeatureFlag::BetaUpdates;
    features.set(FeatureFlag::DebugUI);

    std::cout << "  DarkMode:    " << (features.has(FeatureFlag::DarkMode)    ? "on" : "off") << '\n';
    std::cout << "  Telemetry:   " << (features.has(FeatureFlag::Telemetry)   ? "on" : "off") << '\n';
    std::cout << "  BetaUpdates: " << (features.has(FeatureFlag::BetaUpdates) ? "on" : "off") << '\n';
    std::cout << "  DebugUI:     " << (features.has(FeatureFlag::DebugUI)     ? "on" : "off") << '\n';

    // -------------------------------------------------------------------------
    // 9. Raw bits access
    // -------------------------------------------------------------------------
    std::cout << "\n--- Demo 9: underlying bits ---\n";

    Flags<Permission> raw = Permission::Read | Permission::Execute;
    std::cout << "  raw bits: 0x" << std::hex << raw.bits() << std::dec << '\n';

    return 0;
}
