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

struct NoDef
{
    explicit NoDef(int v) : value(v) {}
    NoDef() = delete;
    bool operator==(const NoDef& o) const { return value == o.value; }
    int value;
};

struct Config
{
    explicit Config(std::string path, int timeout)
        : path(std::move(path)), timeout(timeout) {}
    Config() = delete;

    std::string path;
    int         timeout;
};

// ============================================================================
// Construction
// ============================================================================

TEST(Result_Construction, OkHoldsValue)
{
    const auto r = Result<int, std::string>::ok(42);
    EXPECT_TRUE(r.has_value());
    EXPECT_FALSE(r.has_error());
    EXPECT_EQ(*r, 42);
}

TEST(Result_Construction, ErrHoldsError)
{
    const auto r = Result<int, std::string>::err("oops");
    EXPECT_FALSE(r.has_value());
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), "oops");
}

TEST(Result_Construction, NonDefaultConstructibleT)
{
    const auto r = Result<NoDef, std::string>::ok(NoDef{ 7 });
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(r->value, 7);
}

TEST(Result_Construction, NonDefaultConstructibleConfig)
{
    const auto r = Result<Config, std::string>::ok(Config{ "app.cfg", 30 });
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(r->path, "app.cfg");
    EXPECT_EQ(r->timeout, 30);
}

TEST(Result_Construction, NonDefaultConstructibleErr)
{
    const auto r = Result<Config, std::string>::err("not found");
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), "not found");
}

TEST(Result_Construction, SameTypeForTAndE)
{
    const auto ok  = Result<int, int>::ok(1);
    const auto err = Result<int, int>::err(2);
    EXPECT_TRUE(ok.has_value());
    EXPECT_TRUE(err.has_error());
    EXPECT_EQ(*ok,        1);
    EXPECT_EQ(err.error(), 2);
}

// ============================================================================
// State query
// ============================================================================

TEST(Result_StateQuery, BoolConversionOk)
{
    const auto r = Result<int, std::string>::ok(0);
    EXPECT_TRUE(static_cast<bool>(r));
}

TEST(Result_StateQuery, BoolConversionErr)
{
    const auto r = Result<int, std::string>::err("e");
    EXPECT_FALSE(static_cast<bool>(r));
}

// ============================================================================
// Value access
// ============================================================================

TEST(Result_ValueAccess, ValueOnOk)
{
    auto r = Result<int, std::string>::ok(10);
    EXPECT_EQ(r.value(), 10);
}

TEST(Result_ValueAccess, ValueOnErrThrows)
{
    const auto r = Result<int, std::string>::err("e");
    EXPECT_THROW({ (void)r.value(); }, bad_result_access);
}

TEST(Result_ValueAccess, DerefOperator)
{
    const auto r = Result<int, std::string>::ok(5);
    EXPECT_EQ(*r, 5);
}

TEST(Result_ValueAccess, ArrowOperator)
{
    const auto r = Result<std::string, int>::ok("hello");
    EXPECT_EQ(r->size(), 5u);
}

TEST(Result_ValueAccess, ValueOrOnOk)
{
    const auto r = Result<int, std::string>::ok(3);
    EXPECT_EQ(r.value_or(99), 3);
}

TEST(Result_ValueAccess, ValueOrOnErr)
{
    const auto r = Result<int, std::string>::err("e");
    EXPECT_EQ(r.value_or(99), 99);
}

// ============================================================================
// Error access
// ============================================================================

TEST(Result_ErrorAccess, ErrorOnErr)
{
    const auto r = Result<int, std::string>::err("fail");
    EXPECT_EQ(r.error(), "fail");
}

TEST(Result_ErrorAccess, ErrorOnOkThrows)
{
    const auto r = Result<int, std::string>::ok(1);
    EXPECT_THROW({ (void)r.error(); }, bad_result_access);
}

TEST(Result_ErrorAccess, ErrorOrOnErr)
{
    const auto r = Result<int, std::string>::err("e");
    EXPECT_EQ(r.error_or("fallback"), "e");
}

TEST(Result_ErrorAccess, ErrorOrOnOk)
{
    const auto r = Result<int, std::string>::ok(1);
    EXPECT_EQ(r.error_or("fallback"), "fallback");
}

// ============================================================================
// transform
// ============================================================================

TEST(Result_Transform, TransformOnOk)
{
    const auto r = Result<int, std::string>::ok(4)
        .transform([](int n) { return n * 2; });
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(*r, 8);
}

TEST(Result_Transform, TransformOnErr)
{
    const auto r = Result<int, std::string>::err("e")
        .transform([](int n) { return n * 2; });
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), "e");
}

TEST(Result_Transform, TransformChangesType)
{
    const auto r = Result<int, std::string>::ok(42)
        .transform([](int n) { return std::to_string(n); });
    EXPECT_EQ(*r, "42");
}

TEST(Result_Transform, TransformRvalue)
{
    auto r = Result<std::string, int>::ok("hello");
    const auto r2 = std::move(r).transform([](std::string s) { return s.size(); });
    EXPECT_EQ(*r2, 5u);
}

TEST(Result_Transform, TransformDecaysConstReturn)
{
    // Callable returns const int -- decay_t must strip the const so the
    // stored type is plain int, not const int.
    const auto r = Result<int, std::string>::ok(7)
        .transform([](int n) -> int { return n; });
    static_assert(std::is_same_v<std::decay_t<decltype(r.value())>, int>,
                  "transform must decay const from return type");
    EXPECT_EQ(r.value(), 7);
}

TEST(Result_Transform, TransformDecaysReferenceReturn)
{
    // Callable returns int& -- decay_t must strip the reference so the
    // stored type is a value, not a dangling reference.
    static int global = 99;
    const auto r = Result<int, std::string>::ok(0)
        .transform([](int) -> int& { return global; });
    EXPECT_EQ(r.value(), 99);
}

TEST(Result_Transform, TransformNonDefaultConstructible)
{
    const auto r = Result<Config, std::string>::ok(Config{ "app.cfg", 30 })
        .transform([](const Config& c) { return c.timeout; });
    EXPECT_EQ(r.value(), 30);
}

// ============================================================================
// transform_error
// ============================================================================

TEST(Result_TransformError, TransformErrorOnErr)
{
    const auto r = Result<int, std::string>::err("oops")
        .transform_error([](const std::string& e) { return e.size(); });
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), 4u);
}

TEST(Result_TransformError, TransformErrorOnOk)
{
    const auto r = Result<int, std::string>::ok(1)
        .transform_error([](const std::string& e) { return e.size(); });
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(*r, 1);
}

TEST(Result_TransformError, TransformErrorDecaysConstReturn)
{
    const auto r = Result<int, std::string>::err("e")
        .transform_error([](const std::string& e) -> std::size_t { return e.size(); });
    static_assert(std::is_same_v<std::decay_t<decltype(r.error())>, std::size_t>,
                  "transform_error must decay const from return type");
    EXPECT_EQ(r.error(), 1u);
}

// ============================================================================
// and_then
// ============================================================================

TEST(Result_AndThen, AndThenOnOk)
{
    const auto r = Result<int, std::string>::ok(5)
        .and_then([](int n) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(n + 1);
        });
    EXPECT_EQ(*r, 6);
}

TEST(Result_AndThen, AndThenOnErr)
{
    const auto r = Result<int, std::string>::err("e")
        .and_then([](int n) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(n + 1);
        });
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), "e");
}

TEST(Result_AndThen, AndThenShortCircuits)
{
    int calls = 0;
    (void)Result<int, std::string>::err("e")
        .and_then([&](int n) -> Result<int, std::string>
        {
            ++calls;
            return Result<int, std::string>::ok(n);
        });
    EXPECT_EQ(calls, 0);
}

TEST(Result_AndThen, AndThenChain)
{
    const auto r = Result<int, std::string>::ok(2)
        .and_then([](int n) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(n * 3);
        })
        .and_then([](int n) -> Result<std::string, std::string>
        {
            return Result<std::string, std::string>::ok(std::to_string(n));
        });
    EXPECT_EQ(*r, "6");
}

// ============================================================================
// or_else
// ============================================================================

TEST(Result_OrElse, OrElseOnErr)
{
    const auto r = Result<int, std::string>::err("e")
        .or_else([](const std::string&) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(0);
        });
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(*r, 0);
}

TEST(Result_OrElse, OrElseOnOk)
{
    const auto r = Result<int, std::string>::ok(7)
        .or_else([](const std::string&) -> Result<int, std::string>
        {
            return Result<int, std::string>::ok(0);
        });
    EXPECT_EQ(*r, 7);
}

TEST(Result_OrElse, OrElseShortCircuits)
{
    int calls = 0;
    (void)Result<int, std::string>::ok(1)
        .or_else([&](const std::string&) -> Result<int, std::string>
        {
            ++calls;
            return Result<int, std::string>::ok(0);
        });
    EXPECT_EQ(calls, 0);
}

// ============================================================================
// inspect / inspect_error
// ============================================================================

TEST(Result_Inspect, InspectCalledOnOk)
{
    int seen = 0;
    Result<int, std::string>::ok(3)
        .inspect([&](int n) { seen = n; });
    EXPECT_EQ(seen, 3);
}

TEST(Result_Inspect, InspectNotCalledOnErr)
{
    int seen = 0;
    Result<int, std::string>::err("e")
        .inspect([&](int n) { seen = n; });
    EXPECT_EQ(seen, 0);
}

TEST(Result_Inspect, InspectErrorCalledOnErr)
{
    std::string seen;
    Result<int, std::string>::err("fail")
        .inspect_error([&](const std::string& e) { seen = e; });
    EXPECT_EQ(seen, "fail");
}

TEST(Result_Inspect, InspectErrorNotCalledOnOk)
{
    std::string seen;
    Result<int, std::string>::ok(1)
        .inspect_error([&](const std::string& e) { seen = e; });
    EXPECT_TRUE(seen.empty());
}

TEST(Result_Inspect, InspectReturnsRefForChaining)
{
    int seen = 0;
    const auto r = Result<int, std::string>::ok(9)
        .inspect([&](int n) { seen = n; })
        .transform([](int n) { return n + 1; });
    EXPECT_EQ(seen, 9);
    EXPECT_EQ(*r, 10);
}

// ============================================================================
// Comparison
// ============================================================================

TEST(Result_Comparison, EqualOk)
{
    const auto a = Result<int, std::string>::ok(1);
    const auto b = Result<int, std::string>::ok(1);
    EXPECT_EQ(a, b);
}

TEST(Result_Comparison, UnequalOkDifferentValue)
{
    const auto a = Result<int, std::string>::ok(1);
    const auto b = Result<int, std::string>::ok(2);
    EXPECT_NE(a, b);
}

TEST(Result_Comparison, EqualErr)
{
    const auto a = Result<int, std::string>::err("e");
    const auto b = Result<int, std::string>::err("e");
    EXPECT_EQ(a, b);
}

TEST(Result_Comparison, OkAndErrNotEqual)
{
    const auto a = Result<int, std::string>::ok(1);
    const auto b = Result<int, std::string>::err("e");
    EXPECT_NE(a, b);
}

// ============================================================================
// enum as E
// ============================================================================

enum class ParseError { InvalidInput, Overflow };

TEST(Result_EnumError, EnumAsErrorType)
{
    const auto r = Result<int, ParseError>::err(ParseError::InvalidInput);
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), ParseError::InvalidInput);
}

TEST(Result_EnumError, EnumPropagatesThroughTransform)
{
    const auto r = Result<int, ParseError>::err(ParseError::Overflow)
        .transform([](int n) { return n * 2; });
    EXPECT_TRUE(r.has_error());
    EXPECT_EQ(r.error(), ParseError::Overflow);
}
