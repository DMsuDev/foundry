// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include <foundry/version.h>

#include <gtest/gtest.h>

// ============================================================================
// Version Components
// ============================================================================

TEST(Version, Components)
{
    EXPECT_EQ(FOUNDRY_VERSION_MAJOR, foundry::version::major);
    EXPECT_EQ(FOUNDRY_VERSION_MINOR, foundry::version::minor);
    EXPECT_EQ(FOUNDRY_VERSION_PATCH, foundry::version::patch);
}

// ============================================================================
// Version Encoding
// ============================================================================

TEST(Version, Encode)
{
    EXPECT_EQ(foundry::version::encode(0, 1, 0), 100u);
    EXPECT_EQ(foundry::version::encode(1, 2, 3), 10203u);
    EXPECT_EQ(foundry::version::encode(12, 34, 56), 123456u);
}

TEST(Version, Value)
{
    EXPECT_EQ(
        foundry::version::value,
        foundry::version::encode(
            foundry::version::major,
            foundry::version::minor,
            foundry::version::patch
        )
    );
}

// ============================================================================
// Version Comparison
// ============================================================================

TEST(Version, AtLeast)
{
    EXPECT_TRUE(
        foundry::version::at_least(
            foundry::version::major,
            foundry::version::minor,
            foundry::version::patch
        )
    );

    EXPECT_TRUE(
        foundry::version::at_least(
            0,
            0,
            0
        )
    );
}

// ============================================================================
// Version Strings
// ============================================================================

TEST(Version, Strings)
{
    EXPECT_EQ(foundry::version::string, FOUNDRY_VERSION_STRING);
    EXPECT_EQ(foundry::version::full_string, FOUNDRY_VERSION_FULL);

    EXPECT_FALSE(foundry::version::string.empty());
    EXPECT_FALSE(foundry::version::full_string.empty());
}

// ============================================================================
// Compile-Time API
// ============================================================================

TEST(Version, MacroEncoding)
{
    EXPECT_EQ(FOUNDRY_VERSION_ENCODE(1, 2, 3), 10203u);
    EXPECT_EQ(FOUNDRY_VERSION, foundry::version::value);
}

TEST(Version, MacroAtLeast)
{
#if FOUNDRY_VERSION_AT_LEAST(0, 0, 0)
    SUCCEED();
#else
    FAIL() << "Current version should be at least 0.0.0.";
#endif
}
