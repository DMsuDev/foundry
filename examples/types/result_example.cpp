// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/types/result.h"

#include <iostream>
#include <string>
#include <string_view>

using foundry::types::Result;

// ============================================================================
// Helpers
// ============================================================================

static Result<int, std::string> parse_int(std::string_view s)
{
    try
    {
        return Result<int, std::string>::ok(std::stoi(std::string(s)));
    }
    catch (...)
    {
        return Result<int, std::string>::err(std::string("not a valid integer: ") + std::string(s));
    }
}

static Result<int, std::string> check_positive(int n)
{
    if (n <= 0)
        return Result<int, std::string>::err("value must be positive");
    return Result<int, std::string>::ok(n);
}

static Result<std::string, std::string> int_to_string(int n)
{
    return Result<std::string, std::string>::ok(std::to_string(n));
}

// ============================================================================
// Demos
// ============================================================================

static void Demo_BasicUsage();
static void Demo_ValueOrFallback();
static void Demo_MonadicChaining();
static void Demo_ErrorTransformation();
static void Demo_Inspection();
static void Demo_BadResultAccess();

// ============================================================================
// Entry Point
// ============================================================================

int main()
{
    std::cout << "==========================================\n";
    std::cout << "        Foundry Result Examples           \n";
    std::cout << "==========================================\n";

    Demo_BasicUsage();
    Demo_ValueOrFallback();
    Demo_MonadicChaining();
    Demo_ErrorTransformation();
    Demo_Inspection();
    Demo_BadResultAccess();

    return 0;
}

// ============================================================================
// Demo 1: Basic Usage
// ============================================================================

// Demonstrates constructing ok/err results and querying their state.
static void Demo_BasicUsage()
{
    std::cout << "\n--- Demo 1: Basic Usage ---\n";

    const auto ok  = parse_int("42");
    const auto err = parse_int("bad");

    std::cout << "parse_int(\"42\"):\n";
    std::cout << "  has_value() = " << (ok.has_value() ? "true" : "false") << "\n";
    std::cout << "  *result     = " << *ok << "\n";

    std::cout << "parse_int(\"bad\"):\n";
    std::cout << "  is_error()  = " << (err.is_error() ? "true" : "false") << "\n";
    std::cout << "  error()     = " << err.error() << "\n";

    // Contextual bool conversion
    if (ok)  std::cout << "ok result evaluates to true\n";
    if (!err) std::cout << "error result evaluates to false\n";
}

// ============================================================================
// Demo 2: value_or Fallback
// ============================================================================

// Demonstrates value_or() as a safe extraction without branching.
static void Demo_ValueOrFallback()
{
    std::cout << "\n--- Demo 2: value_or Fallback ---\n";

    std::cout << "parse_int(\"10\").value_or(0)  = " << parse_int("10").value_or(0)  << "\n";
    std::cout << "parse_int(\"bad\").value_or(0) = " << parse_int("bad").value_or(0) << "\n";
}

// ============================================================================
// Demo 3: Monadic Chaining
// ============================================================================

// Demonstrates map() and and_then() for composing operations that may fail,
// without nested if-blocks or manual error checks at each step.
static void Demo_MonadicChaining()
{
    std::cout << "\n--- Demo 3: Monadic Chaining ---\n";

    // map() transforms the value; and_then() chains a Result-returning function.
    // Errors short-circuit the chain — subsequent steps are skipped.

    const auto success = parse_int("21")
        .map([](int n) { return n * 2; })
        .and_then(check_positive)
        .and_then(int_to_string);

    const auto failed_parse = parse_int("bad")
        .map([](int n) { return n * 2; })
        .and_then(check_positive)
        .and_then(int_to_string);

    const auto failed_check = parse_int("-5")
        .map([](int n) { return n * 2; })
        .and_then(check_positive)
        .and_then(int_to_string);

    std::cout << "parse(\"21\") * 2 -> check_positive -> to_string:\n";
    std::cout << "  " << (success ? success.value() : "[error] " + success.error()) << "\n";

    std::cout << "parse(\"bad\") -> ...\n";
    std::cout << "  " << (failed_parse ? failed_parse.value() : "[error] " + failed_parse.error()) << "\n";

    std::cout << "parse(\"-5\") * 2 -> check_positive -> ...\n";
    std::cout << "  " << (failed_check ? failed_check.value() : "[error] " + failed_check.error()) << "\n";
}

// ============================================================================
// Demo 4: Error Transformation
// ============================================================================

// Demonstrates map_error() to convert an error type, and or_else() to
// recover from an error by providing a fallback Result.
static void Demo_ErrorTransformation()
{
    std::cout << "\n--- Demo 4: Error Transformation ---\n";

    // map_error: convert error type from std::string to std::size_t (error length)
    const auto length = parse_int("bad")
        .map_error([](const std::string& e) { return e.size(); });

    std::cout << "parse(\"bad\").map_error(size): error length = " << length.error() << "\n";

    // or_else: recover from any parse failure with a default value
    const auto recovered = parse_int("bad")
        .or_else([](const std::string&) { return Result<int, std::string>::ok(0); });

    std::cout << "parse(\"bad\").or_else(default 0): value = " << recovered.value() << "\n";
}

// ============================================================================
// Demo 5: Inspection
// ============================================================================

// Demonstrates inspect() and inspect_error() for side-effects (e.g. logging)
// without breaking the chain or extracting the value.
static void Demo_Inspection()
{
    std::cout << "\n--- Demo 5: Inspection ---\n";

    parse_int("42")
        .inspect([](int n)
        {
            std::cout << "  [inspect] got value: " << n << "\n";
        })
        .inspect_error([](const std::string& e)
        {
            std::cout << "  [inspect_error] got error: " << e << "\n";
        });

    parse_int("bad")
        .inspect([](int n)
        {
            std::cout << "  [inspect] got value: " << n << "\n";
        })
        .inspect_error([](const std::string& e)
        {
            std::cout << "  [inspect_error] got error: " << e << "\n";
        });
}

// ============================================================================
// Demo 6: bad_result_access
// ============================================================================

// Demonstrates that accessing the wrong side of a Result throws
// foundry::bad_result_access, which can be caught like any standard exception.
static void Demo_BadResultAccess()
{
    std::cout << "\n--- Demo 6: bad_result_access ---\n";

    const auto err = parse_int("bad");

    try
    {
        [[maybe_unused]] const int n = err.value();
    }
    catch (const foundry::bad_result_access& e)
    {
        std::cout << "Caught foundry::bad_result_access: " << e.what() << "\n";
    }
}
