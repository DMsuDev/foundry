// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/platform/paths.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

// =============================================================================
//  working_directory
// =============================================================================

TEST(WorkingDirectoryTest, ReturnsNonEmptyPath)
{
    auto path = foundry::platform::working_directory();
    EXPECT_FALSE(path.empty());
}

TEST(WorkingDirectoryTest, PathIsAbsolute)
{
    auto path = foundry::platform::working_directory();
    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(path.is_absolute());
}

TEST(WorkingDirectoryTest, DirectoryExists)
{
    auto path = foundry::platform::working_directory();
    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_directory(path));
}

// =============================================================================
//  executable_path
// =============================================================================

TEST(ExecutablePathTest, DoesNotThrow)
{
    EXPECT_NO_THROW(foundry::platform::executable_path());
}

TEST(ExecutablePathTest, ReturnsNonEmptyPath)
{
    auto path = foundry::platform::executable_path();
    EXPECT_FALSE(path.empty());
}

TEST(ExecutablePathTest, PathIsAbsolute)
{
    auto path = foundry::platform::executable_path();
    EXPECT_TRUE(path.is_absolute());
}

TEST(ExecutablePathTest, FileExists)
{
    auto path = foundry::platform::executable_path();
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

// =============================================================================
//  executable_directory
// =============================================================================

TEST(ExecutableDirectoryTest, DoesNotThrow)
{
    EXPECT_NO_THROW(foundry::platform::executable_directory());
}

TEST(ExecutableDirectoryTest, IsParentOfExecutablePath)
{
    auto dir  = foundry::platform::executable_directory();
    auto exe  = foundry::platform::executable_path();
    EXPECT_EQ(dir, exe.parent_path());
}

TEST(ExecutableDirectoryTest, DirectoryExists)
{
    auto dir = foundry::platform::executable_directory();
    EXPECT_TRUE(std::filesystem::exists(dir));
    EXPECT_TRUE(std::filesystem::is_directory(dir));
}

// =============================================================================
//  temp_directory
// =============================================================================

TEST(TempDirectoryTest, ReturnsNonEmptyPath)
{
    auto path = foundry::platform::temp_directory();
    EXPECT_FALSE(path.empty());
}

TEST(TempDirectoryTest, PathIsAbsolute)
{
    auto path = foundry::platform::temp_directory();
    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(path.is_absolute());
}

TEST(TempDirectoryTest, DirectoryExists)
{
    auto path = foundry::platform::temp_directory();
    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_directory(path));
}

TEST(TempDirectoryTest, IsWritable)
{
    // Verify we can actually create a file in the temp directory,
    // since "exists" alone doesn't guarantee write permission.
    auto tmp  = foundry::platform::temp_directory();
    ASSERT_FALSE(tmp.empty());

    auto probe = tmp / "foundry_temp_probe.tmp";

    std::error_code ec;
    {
        // Create the file
        std::ofstream f(probe);
        ASSERT_TRUE(f.is_open()) << "Could not create probe file in temp dir";
    }

    // Cleanup — don't leave artefacts on the test machine
    std::filesystem::remove(probe, ec);
}
