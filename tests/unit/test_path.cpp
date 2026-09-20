// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/platform/paths.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <system_error>

// =============================================================================
//  working_directory
// =============================================================================

TEST(WorkingDirectoryTest, ReturnsNonEmptyPath)
{
    const auto path = foundry::platform::working_directory();

    EXPECT_FALSE(path.empty());
}

TEST(WorkingDirectoryTest, PathIsAbsolute)
{
    const auto path = foundry::platform::working_directory();

    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(path.is_absolute());
}

TEST(WorkingDirectoryTest, DirectoryExists)
{
    const auto path = foundry::platform::working_directory();

    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_directory(path));
}

TEST(WorkingDirectoryTest, ErrorCodeOverloadSucceeds)
{
    std::error_code ec;

    const auto path = foundry::platform::working_directory(ec);

    EXPECT_FALSE(ec);
    EXPECT_FALSE(path.empty());
}


// =============================================================================
//  executable_path
// =============================================================================

TEST(ExecutablePathTest, ReturnsNonEmptyPath)
{
    const auto path = foundry::platform::executable_path();

    EXPECT_FALSE(path.empty());
}

TEST(ExecutablePathTest, PathIsAbsolute)
{
    const auto path = foundry::platform::executable_path();

    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(path.is_absolute());
}

TEST(ExecutablePathTest, FileExists)
{
    const auto path = foundry::platform::executable_path();

    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

TEST(ExecutablePathTest, ErrorCodeOverloadSucceeds)
{
    std::error_code ec;

    const auto path = foundry::platform::executable_path(ec);

    EXPECT_FALSE(ec);
    EXPECT_FALSE(path.empty());
}


// =============================================================================
//  executable_directory
// =============================================================================

TEST(ExecutableDirectoryTest, MatchesExecutableParent)
{
    const auto directory = foundry::platform::executable_directory();
    const auto executable = foundry::platform::executable_path();

    ASSERT_FALSE(directory.empty());
    ASSERT_FALSE(executable.empty());

    EXPECT_EQ(directory, executable.parent_path());
}

TEST(ExecutableDirectoryTest, DirectoryExists)
{
    const auto directory = foundry::platform::executable_directory();

    ASSERT_FALSE(directory.empty());
    EXPECT_TRUE(std::filesystem::exists(directory));
    EXPECT_TRUE(std::filesystem::is_directory(directory));
}

TEST(ExecutableDirectoryTest, ErrorCodeOverloadSucceeds)
{
    std::error_code ec;

    const auto directory = foundry::platform::executable_directory(ec);

    EXPECT_FALSE(ec);
    EXPECT_FALSE(directory.empty());
}


// =============================================================================
//  temp_directory
// =============================================================================

TEST(TempDirectoryTest, ReturnsNonEmptyPath)
{
    const auto path = foundry::platform::temp_directory();

    EXPECT_FALSE(path.empty());
}

TEST(TempDirectoryTest, PathIsAbsolute)
{
    const auto path = foundry::platform::temp_directory();

    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(path.is_absolute());
}

TEST(TempDirectoryTest, DirectoryExists)
{
    const auto path = foundry::platform::temp_directory();

    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_directory(path));
}

TEST(TempDirectoryTest, IsWritable)
{
    // Verify we can actually create a file in the temp directory,
    // since "exists" alone doesn't guarantee write permission.
    const auto tmp = foundry::platform::temp_directory();
    ASSERT_FALSE(tmp.empty());

    const auto probe = tmp / "foundry_temp_probe.tmp";

    std::error_code ec;
    {
        std::ofstream file(probe);
        ASSERT_TRUE(file.is_open()) << "Could not create probe file in temp directory";
    }

    // Cleanup; don't leave artifacts on the test machine
    std::filesystem::remove(probe, ec);
    EXPECT_FALSE(ec);
}

TEST(TempDirectoryTest, ErrorCodeOverloadSucceeds)
{
    std::error_code ec;

    const auto path = foundry::platform::temp_directory(ec);

    EXPECT_FALSE(ec);
    EXPECT_FALSE(path.empty());
}
