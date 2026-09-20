// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/types/semantic_version.h"

// Provides access to the current Foundry library version
#include "foundry/version.h"

#include <iostream>
#include <string_view>

// ============================================================================
// Demos
// ============================================================================

static void Demo_Construction();
static void Demo_Parsing();
static void Demo_Comparison();
static void Demo_BuildMetadata();
static void Demo_VersionGate();

// ============================================================================
// Entry Point
// ============================================================================

int main()
{
    std::cout << "==========================================\n";
    std::cout << "    Foundry SemanticVersion Examples      \n";
    std::cout << "==========================================\n";

    // Print the current version of the Foundry library
    std::cout << "\n--- Current Foundry Version ---\n";
    std::cout << "Current version: " << foundry::version::current.to_string() << "\n";
    std::cout << "Is pre-release:  " << (foundry::version::current.is_prerelease() ? "true" : "false") << "\n";

    Demo_Construction();
    Demo_Parsing();
    Demo_Comparison();
    Demo_BuildMetadata();
    Demo_VersionGate();

    return 0;
}

// ============================================================================
// Demo 1: Direct Construction
// ============================================================================

// Demonstrates constructing a SemanticVersion directly from its components.
// Use this when the version is known at compile time or comes from trusted input.
static void Demo_Construction()
{
    std::cout << "\n--- Demo 1: Direct Construction ---\n";

    const foundry::types::SemanticVersion stable{1, 2, 3};
    const foundry::types::SemanticVersion prerelease{1, 2, 3, "alpha.1"};
    const foundry::types::SemanticVersion with_build{1, 2, 3, "rc.1", "build.42"};

    std::cout << "Stable:      " << stable.to_string()      << "\n";
    std::cout << "Pre-release: " << prerelease.to_string()  << "\n";
    std::cout << "With build:  " << with_build.to_string()  << "\n";
    std::cout << "Core only:   " << with_build.core_string() << "\n";

    std::cout << "Is pre-release: " << (prerelease.is_prerelease() ? "true" : "false") << "\n";
    std::cout << "Has metadata:   " << (with_build.has_build_metadata() ? "true" : "false") << "\n";
}

// ============================================================================
// Demo 2: Parsing External Input
// ============================================================================

// Demonstrates parse() for validating and consuming version strings from
// external sources such as config files, API responses, or CLI arguments.
static void Demo_Parsing()
{
    std::cout << "\n--- Demo 2: Parsing External Input ---\n";

    const std::string_view inputs[] = {
        "1.0.0",
        "2.0.0-alpha.1",
        "3.1.4-rc.1+build.42",
        "1.2.3+build.001",   // Leading zeroes allowed in build metadata
        "bad-version",       // Invalid
        "01.2.3",            // Invalid: leading zero in MAJOR
        "1.2.3-alpha.01",    // Invalid: leading zero in numeric pre-release id
        "1.2.3-",            // Invalid: empty pre-release
    };

    for (std::string_view input : inputs)
    {
        const auto version = foundry::types::SemanticVersion::parse(input);

        if (version.has_value())
        {
            std::cout << "[OK]    \"" << input << "\" -> " << version->to_string() << "\n";
        }
        else
        {
            std::cout << "[FAIL]  \"" << input << "\" -> invalid SemVer string\n";
        }
    }
}

// ============================================================================
// Demo 3: Precedence and Comparison
// ============================================================================

// Demonstrates the official SemVer 2.0.0 precedence sequence.
// Pre-release versions have lower precedence than the associated release.
static void Demo_Comparison()
{
    std::cout << "\n--- Demo 3: Precedence and Comparison ---\n";

    // Official precedence sequence from semver.org
    const foundry::types::SemanticVersion sequence[] = {
        {1, 0, 0, "alpha"},
        {1, 0, 0, "alpha.1"},
        {1, 0, 0, "alpha.beta"},
        {1, 0, 0, "beta"},
        {1, 0, 0, "beta.2"},
        {1, 0, 0, "beta.11"},
        {1, 0, 0, "rc.1"},
        {1, 0, 0},
    };

    std::cout << "Official SemVer precedence sequence:\n";
    for (std::size_t i = 0; i + 1 < std::size(sequence); ++i)
    {
        const auto& lhs = sequence[i];
        const auto& rhs = sequence[i + 1];
        std::cout << "  " << lhs.to_string() << " < " << rhs.to_string()
                  << " -> " << (lhs < rhs ? "true" : "false") << "\n";
    }

    std::cout << "\nDirect comparisons:\n";
    const foundry::types::SemanticVersion v100{1, 0, 0};
    const foundry::types::SemanticVersion v110{1, 1, 0};
    const foundry::types::SemanticVersion v200{2, 0, 0};

    std::cout << "  1.0.0 < 1.1.0 -> " << (v100 < v110 ? "true" : "false") << "\n";
    std::cout << "  1.1.0 < 2.0.0 -> " << (v110 < v200 ? "true" : "false") << "\n";
    std::cout << "  2.0.0 > 1.0.0 -> " << (v200 > v100 ? "true" : "false") << "\n";
    std::cout << "  1.0.0 == 1.0.0 -> " << (v100 == v100 ? "true" : "false") << "\n";
}

// ============================================================================
// Demo 4: Build Metadata
// ============================================================================

// Demonstrates that build metadata is preserved in to_string() but
// excluded from all comparisons, as specified by SemVer 2.0.0.
static void Demo_BuildMetadata()
{
    std::cout << "\n--- Demo 4: Build Metadata ---\n";

    const auto v1 = foundry::types::SemanticVersion::parse("1.0.0+build.1");
    const auto v2 = foundry::types::SemanticVersion::parse("1.0.0+build.2");

    std::cout << "v1: " << v1->to_string() << "\n";
    std::cout << "v2: " << v2->to_string() << "\n";

    std::cout << "v1 == v2 (metadata excluded): " << (*v1 == *v2 ? "true" : "false") << "\n";
    std::cout << "v1 to_string (with build):    " << v1->to_string(true)  << "\n";
    std::cout << "v1 to_string (without build): " << v1->to_string(false) << "\n";
}

// ============================================================================
// Demo 5: Version Gate (at_least)
// ============================================================================

// Demonstrates at_least() as a runtime version check, useful for checking
// library or plugin versions against a minimum required version.
static void Demo_VersionGate()
{
    std::cout << "\n--- Demo 5: Version Gate (at_least) ---\n";

    const foundry::types::SemanticVersion current{2, 3, 1};

    std::cout << "Current version: " << current.to_string() << "\n";

    const auto check = [&](std::uint32_t major, std::uint32_t minor, std::uint32_t patch)
    {
        std::cout << "  at_least(" << major << ", " << minor << ", " << patch << ") -> "
                  << (current.at_least(major, minor, patch) ? "true" : "false") << "\n";
    };

    check(2, 0, 0);   // true  — same major, lower minor
    check(2, 3, 0);   // true  — same major.minor, lower patch
    check(2, 3, 1);   // true  — exact match
    check(2, 3, 2);   // false — patch too high
    check(2, 4, 0);   // false — minor too high
    check(3, 0, 0);   // false — major too high
    check(1, 9, 9);   // true  — lower major
}
