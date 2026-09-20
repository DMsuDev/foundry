// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/types/result.h"

#include <gtest/gtest.h>
#include <string>

using foundry::bad_result_access;
using foundry::types::Result;

// ============================================================================
// Helpers
// ============================================================================

static Result<int, std::string> parse_int(std::string_view s)
{
    try   { return Result<int, std::string>::ok(std::stoi(std::string(s))); }
    catch (...) { return Result<int, std::string>::err("not a valid integer"); }
}

// ============================================================================
// Construction
// ============================================================================

TEST(ResultTest, OkHasValue)
{
    const auto r = Result<int, std::string>::ok(42);
    EXPECT_TRUE(r.has_value());
    EXPECT_FALSE(r.is_error());
}

TEST(ResultTest, ErrHasError)
{
    const auto r = Result<int, std::string>::err("oops");
    EXPECT_FALSE(r.has_value());
    EXPECT_TRUE(r.is_error());
}

TEST(ResultTest, OkEvaluatesToTrue)
{
    const auto r = Result<int, std::string>::ok(1);
    EXPECT_TRUE(static_cast<bool>(r));
}

TEST(ResultTest, ErrEvaluatesToFalse)
{
    const auto r = Result<int, std::string>::err("e");
    EXPECT_FALSE(static_cast<bool>(r));
}

// ============================================================================
// Value Access
// ============================================================================

TEST(ResultTest, ValueReturnsContainedValue)
{
    const auto r = Result<int, std::string>::ok(42);
    EXPECT_EQ(r.value(), 42);
}

TEST(ResultTest, DereferenceReturnsValue)
{
    const auto r = Result<int, std::string>::ok(42);
    EXPECT_EQ(*r, 42);
}

TEST(ResultTest, ArrowOperatorAccessesMembers)
{
    const auto r = Result<std::string, std::string>::ok("hello");
    EXPECT_EQ(r->size(), 5u);
}

TEST(ResultTest, ValueThrowsOnError)
{
    const auto r = Result<int, std::string>::err("oops");
    EXPECT_THROW(r.value(), bad_result_access);
}

TEST(ResultTest, ValueOrReturnsValueWhenOk)
{
    const auto r = Result<int, std::string>::ok(42);
    EXPECT_EQ(r.value_or(0), 42);
}

TEST(ResultTest, ValueOrReturnsFallbackWhenError)
{
    const auto r = Result<int, std::string>::err("oops");
    EXPECT_EQ(r.value_or(0), 0);
}

// ============================================================================
// Error Access
// ============================================================================

TEST(ResultTest, ErrorReturnsContainedError)
{
    const auto r = Result<int, std::string>::err("oops");
    EXPECT_EQ(r.error(), "oops");
}

TEST(ResultTest, ErrorThrowsOnValue)
{
    const auto r = Result<int, std::string>::ok(42);
    EXPECT_THROW(r.error(), bad_result_access);
}

// ============================================================================
// bad_result_access
// ============================================================================

TEST(ResultTest, BadResultAccessIsStdException)
{
    const auto r = Result<int, std::string>::err("oops");
    try
    {
        [[maybe_unused]] const int n = r.value();
        FAIL() << "Expected bad_result_access to be thrown";
    }
    catch (const bad_result_access& e)
    {
        EXPECT_NE(std::string(e.what()).find("value()"), std::string::npos);
    }
}

// ============================================================================
// map
// ============================================================================

TEST(ResultTest, MapTransformsValue)
{
    const auto r = Result<int, std::string>::ok(10)
        .map([](int n) { return n * 2; });

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 20);
}

TEST(ResultTest, MapForwardsError)
{
    const auto r = Result<int, std::string>::err("oops")
        .map([](int n) { return n * 2; });

    ASSERT_TRUE(r.is_error());
    EXPECT_EQ(r.error(), "oops");
}

TEST(ResultTest, MapCanChangeValueType)
{
    const auto r = Result<int, std::string>::ok(42)
        .map([](int n) { return std::to_string(n); });

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), "42");
}

// ============================================================================
// map_error
// ============================================================================

TEST(ResultTest, MapErrorTransformsError)
{
    const auto r = Result<int, std::string>::err("oops")
        .map_error([](const std::string& e) { return e.size(); });

    ASSERT_TRUE(r.is_error());
    EXPECT_EQ(r.error(), 4u);
}

TEST(ResultTest, MapErrorForwardsValue)
{
    const auto r = Result<int, std::string>::ok(42)
        .map_error([](const std::string& e) { return e.size(); });

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 42);
}

// ============================================================================
// and_then
// ============================================================================

TEST(ResultTest, AndThenChainsOnValue)
{
    const auto r = parse_int("10")
        .and_then([](int n) -> Result<std::string, std::string>
        {
            return Result<std::string, std::string>::ok(std::to_string(n * 2));
        });

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), "20");
}

TEST(ResultTest, AndThenPropagatesError)
{
    const auto r = parse_int("bad")
        .and_then([](int n) -> Result<std::string, std::string>
        {
            return Result<std::string, std::string>::ok(std::to_string(n));
        });

    EXPECT_TRUE(r.is_error());
}

TEST(ResultTest, AndThenShortCircuitsOnFirstError)
{
    int calls = 0;

    const auto r = parse_int("bad")
        .and_then([&](int n) -> Result<int, std::string>
        {
            ++calls;
            return Result<int, std::string>::ok(n);
        })
        .and_then([&](int n) -> Result<int, std::string>
        {
            ++calls;
            return Result<int, std::string>::ok(n);
        });

    EXPECT_TRUE(r.is_error());
    EXPECT_EQ(calls, 0);
}

// ============================================================================
// or_else
// ============================================================================

TEST(ResultTest, OrElseRecoversFromError)
{
    const auto r = parse_int("bad")
        .or_else([](const std::string&) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(0);
        });

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 0);
}

TEST(ResultTest, OrElseForwardsValue)
{
    const auto r = parse_int("42")
        .or_else([](const std::string&) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(0);
        });

    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r.value(), 42);
}

// ============================================================================
// inspect / inspect_error
// ============================================================================

TEST(ResultTest, InspectInvokesCallbackOnValue)
{
    int side = 0;

    Result<int, std::string>::ok(42)
        .inspect([&](int n) { side = n; });

    EXPECT_EQ(side, 42);
}

TEST(ResultTest, InspectDoesNotInvokeOnError)
{
    int side = 0;

    Result<int, std::string>::err("oops")
        .inspect([&](int n) { side = n; });

    EXPECT_EQ(side, 0);
}

TEST(ResultTest, InspectErrorInvokesCallbackOnError)
{
    std::string side;

    Result<int, std::string>::err("oops")
        .inspect_error([&](const std::string& e) { side = e; });

    EXPECT_EQ(side, "oops");
}

TEST(ResultTest, InspectErrorDoesNotInvokeOnValue)
{
    std::string side;

    Result<int, std::string>::ok(42)
        .inspect_error([&](const std::string& e) { side = e; });

    EXPECT_TRUE(side.empty());
}

TEST(ResultTest, InspectReturnsThisForChaining)
{
    int value_side = 0;
    std::string error_side;

    Result<int, std::string>::ok(42)
        .inspect([&](int n) { value_side = n; })
        .inspect_error([&](const std::string& e) { error_side = e; });

    EXPECT_EQ(value_side, 42);
    EXPECT_TRUE(error_side.empty());
}

// ============================================================================
// Comparison
// ============================================================================

TEST(ResultTest, EqualOkResultsAreEqual)
{
    const auto a = Result<int, std::string>::ok(42);
    const auto b = Result<int, std::string>::ok(42);
    EXPECT_EQ(a, b);
}

TEST(ResultTest, DifferentOkResultsAreNotEqual)
{
    const auto a = Result<int, std::string>::ok(42);
    const auto b = Result<int, std::string>::ok(99);
    EXPECT_NE(a, b);
}

TEST(ResultTest, EqualErrResultsAreEqual)
{
    const auto a = Result<int, std::string>::err("oops");
    const auto b = Result<int, std::string>::err("oops");
    EXPECT_EQ(a, b);
}

TEST(ResultTest, OkAndErrAreNotEqual)
{
    const auto ok  = Result<int, std::string>::ok(42);
    const auto err = Result<int, std::string>::err("oops");
    EXPECT_NE(ok, err);
}

// ============================================================================
// Same T and E type
// ============================================================================

TEST(ResultTest, WorksWhenTAndEAreSameType)
{
    const auto ok  = Result<std::string, std::string>::ok("value");
    const auto err = Result<std::string, std::string>::err("error");

    EXPECT_TRUE(ok.has_value());
    EXPECT_EQ(ok.value(), "value");

    EXPECT_TRUE(err.is_error());
    EXPECT_EQ(err.error(), "error");
}
