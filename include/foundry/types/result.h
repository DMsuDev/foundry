// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

/// @file result.h
/// @brief Type-safe error propagation without exceptions.
///
/// `Result<T, E>` represents either a successful value (`T`) or an error (`E`).
/// The API mirrors `std::expected` (C++23) as closely as possible within C++17.
///
/// Use `Result<std::monostate, E>` for operations that succeed with no value.
/// `T` and `E` may be the same type.
///
/// @par Basic usage
/// @code
/// Result<int, std::string> parse_int(std::string_view s)
/// {
///     try   { return Result<int, std::string>::ok(std::stoi(std::string(s))); }
///     catch (...) { return Result<int, std::string>::err("not a valid integer"); }
/// }
///
/// auto r = parse_int("42");
/// if (r)  { use(*r);        }
/// else    { log(r.error()); }
/// @endcode
///
/// @par Monadic chaining
/// @code
/// parse_int("10")
///     .transform([](int n) { return n * 2; })
///     .and_then([](int n) -> Result<std::string, std::string>
///     {
///         if (n > 100) return Result<std::string, std::string>::err("too large");
///         return Result<std::string, std::string>::ok(std::to_string(n));
///     });
/// @endcode

namespace foundry {

// ============================================================================
// bad_result_access
// ============================================================================

/// @brief Exception thrown when accessing a Result in the wrong state.
///
/// Thrown by `value()` on an error result, or `error()` on a success result.
class bad_result_access : public std::logic_error
{
public:
    explicit bad_result_access(const char* what) : std::logic_error(what) {}
};

// ============================================================================
// detail
// ============================================================================

namespace detail {

/// @cond INTERNAL
// Index-tagged wrappers so std::variant<T, E> works even when T == E.
template <typename T> struct OkHolder  { T value; };
template <typename E> struct ErrHolder { E value; };
/// @endcond

} // namespace detail

// ============================================================================
// Result<T, E>
// ============================================================================

namespace types {

/// @brief Holds either a success value (`T`) or an error value (`E`).
///
/// @tparam T Success type. Must be move-constructible and non-void.
/// @tparam E Error type.  Must be move-constructible and non-void.
///
/// Mirrors the `std::expected<T, E>` interface from C++23.
/// Use `Result<std::monostate, E>` for value-less success. `T` and `E` may be the same type.
template <typename T, typename E>
class Result
{
    static_assert(!std::is_void_v<T>, "T must not be void -- use std::monostate for value-less success.");
    static_assert(!std::is_void_v<E>, "E must not be void.");

    using Storage = std::variant<detail::OkHolder<T>, detail::ErrHolder<E>>;

public:
    // -------------------------------------------------------------------------
    // Factory constructors
    // -------------------------------------------------------------------------

    /// @brief Constructs a success result holding @p value.
    template <typename U = T, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
    [[nodiscard]] static Result ok(U&& value)
    {
        return Result(detail::OkHolder<T>{ T(std::forward<U>(value)) });
    }

    /// @brief Constructs an error result holding @p error.
    template <typename G = E, std::enable_if_t<std::is_constructible_v<E, G&&>, int> = 0>
    [[nodiscard]] static Result err(G&& error)
    {
        return Result(detail::ErrHolder<E>{ E(std::forward<G>(error)) });
    }

    // -------------------------------------------------------------------------
    // State query
    // -------------------------------------------------------------------------

    /// @brief Returns `true` if the result holds a value.
    [[nodiscard]] bool has_value() const noexcept { return m_storage.index() == 0; }

    /// @brief Returns `true` if the result holds an error.
    [[nodiscard]] bool has_error() const noexcept { return m_storage.index() == 1; }

    /// @brief Contextual bool conversion -- `true` when `has_value()`.
    explicit operator bool() const noexcept { return has_value(); }

    // -------------------------------------------------------------------------
    // Value access
    // -------------------------------------------------------------------------

    /// @brief Returns a reference to the contained value.
    /// @throws bad_result_access if the result holds an error.
    /// @{
    [[nodiscard]] T& value() &
    {
        if (!has_value()) { throw bad_result_access("called value() on an error Result"); }
        return std::get<0>(m_storage).value;
    }

    [[nodiscard]] const T& value() const&
    {
        if (!has_value()) { throw bad_result_access("called value() on an error Result"); }
        return std::get<0>(m_storage).value;
    }

    [[nodiscard]] T&& value() &&
    {
        if (!has_value()) { throw bad_result_access("called value() on an error Result"); }
        return std::move(std::get<0>(m_storage).value);
    }

    [[nodiscard]] const T&& value() const&&
    {
        if (!has_value()) { throw bad_result_access("called value() on an error Result"); }
        return std::move(std::get<0>(m_storage).value);
    }
    /// @}

    /// @brief Unchecked access to the stored value.
    /// @pre The result must hold a value. Behaviour is undefined otherwise.
    /// @{
    [[nodiscard]] T&       operator*()  &      { return std::get<0>(m_storage).value; }
    [[nodiscard]] const T& operator*()  const& { return std::get<0>(m_storage).value; }
    [[nodiscard]] T&&      operator*()  &&     { return std::move(std::get<0>(m_storage).value); }
    [[nodiscard]] const T&&operator*()  const&&{ return std::move(std::get<0>(m_storage).value); }
    /// @}

    /// @brief Unchecked pointer to the stored value.
    /// @pre The result must hold a value. Behaviour is undefined otherwise.
    /// @{
    [[nodiscard]] T*       operator->()       { return &std::get<0>(m_storage).value; }
    [[nodiscard]] const T* operator->() const { return &std::get<0>(m_storage).value; }
    /// @}

    /// @brief Returns the value, or @p fallback if the result holds an error.
    /// @{
    template <typename U>
    [[nodiscard]] T value_or(U&& fallback) const&
    {
        return has_value() ? std::get<0>(m_storage).value
                           : static_cast<T>(std::forward<U>(fallback));
    }

    template <typename U>
    [[nodiscard]] T value_or(U&& fallback) &&
    {
        return has_value() ? std::move(std::get<0>(m_storage).value)
                           : static_cast<T>(std::forward<U>(fallback));
    }
    /// @}

    // -------------------------------------------------------------------------
    // Error access
    // -------------------------------------------------------------------------

    /// @brief Returns a reference to the contained error.
    /// @throws bad_result_access if the result holds a value.
    /// @{
    [[nodiscard]] E& error() &
    {
        if (!has_error()) { throw bad_result_access("called error() on a success Result"); }
        return std::get<1>(m_storage).value;
    }

    [[nodiscard]] const E& error() const&
    {
        if (!has_error()) { throw bad_result_access("called error() on a success Result"); }
        return std::get<1>(m_storage).value;
    }

    [[nodiscard]] E&& error() &&
    {
        if (!has_error()) { throw bad_result_access("called error() on a success Result"); }
        return std::move(std::get<1>(m_storage).value);
    }

    [[nodiscard]] const E&& error() const&&
    {
        if (!has_error()) { throw bad_result_access("called error() on a success Result"); }
        return std::move(std::get<1>(m_storage).value);
    }
    /// @}

    /// @brief Returns the error, or @p fallback if the result holds a value.
    /// @{
    template <typename G>
    [[nodiscard]] E error_or(G&& fallback) const&
    {
        return has_error() ? std::get<1>(m_storage).value
                           : static_cast<E>(std::forward<G>(fallback));
    }

    template <typename G>
    [[nodiscard]] E error_or(G&& fallback) &&
    {
        return has_error() ? std::move(std::get<1>(m_storage).value)
                           : static_cast<E>(std::forward<G>(fallback));
    }
    /// @}

    // -------------------------------------------------------------------------
    // Monadic operations
    // -------------------------------------------------------------------------

    /// @brief Applies @p f to the value and returns a new `Result<U, E>`.
    ///
    /// Propagates the error unchanged if the result holds one.
    /// Mirrors `std::expected::transform()` (C++23).
    ///
    /// @tparam F Callable: `U f(const T&)` or `U f(T&&)`.
    /// @{
    template <typename F>
    [[nodiscard]] auto transform(F&& f) const& -> Result<std::decay_t<std::invoke_result_t<F, const T&>>, E>
    {
        using U = std::decay_t<std::invoke_result_t<F, const T&>>;
        if (has_value())
        {
            return Result<U, E>::ok(std::invoke(std::forward<F>(f), std::get<0>(m_storage).value));
        }
        return Result<U, E>::err(std::get<1>(m_storage).value);
    }

    template <typename F>
    [[nodiscard]] auto transform(F&& f) && -> Result<std::decay_t<std::invoke_result_t<F, T&&>>, E>
    {
        using U = std::decay_t<std::invoke_result_t<F, T&&>>;
        if (has_value())
        {
            return Result<U, E>::ok(std::invoke(std::forward<F>(f), std::move(std::get<0>(m_storage).value)));
        }
        return Result<U, E>::err(std::move(std::get<1>(m_storage).value));
    }
    /// @}

    /// @brief Applies @p f to the error and returns a new `Result<T, G>`.
    ///
    /// Propagates the value unchanged if the result holds one.
    /// Mirrors `std::expected::transform_error()` (C++23).
    ///
    /// @tparam F Callable: `G f(const E&)` or `G f(E&&)`.
    /// @{
    template <typename F>
    [[nodiscard]] auto transform_error(F&& f) const& -> Result<T, std::decay_t<std::invoke_result_t<F, const E&>>>
    {
        using G = std::decay_t<std::invoke_result_t<F, const E&>>;
        if (has_error())
        {
            return Result<T, G>::err(std::invoke(std::forward<F>(f), std::get<1>(m_storage).value));
        }
        return Result<T, G>::ok(std::get<0>(m_storage).value);
    }

    template <typename F>
    [[nodiscard]] auto transform_error(F&& f) && -> Result<T, std::decay_t<std::invoke_result_t<F, E&&>>>
    {
        using G = std::decay_t<std::invoke_result_t<F, E&&>>;
        if (has_error())
        {
            return Result<T, G>::err(std::invoke(std::forward<F>(f), std::move(std::get<1>(m_storage).value)));
        }
        return Result<T, G>::ok(std::move(std::get<0>(m_storage).value));
    }
    /// @}

    /// @brief Passes the value to @p f and returns its `Result`. Propagates errors.
    ///
    /// Mirrors `std::expected::and_then()` (C++23).
    ///
    /// @tparam F Callable: `Result<U, E> f(const T&)` or `Result<U, E> f(T&&)`.
    /// @{
    template <typename F>
    [[nodiscard]] auto and_then(F&& f) const& -> std::invoke_result_t<F, const T&>
    {
        using R = std::invoke_result_t<F, const T&>;
        if (has_value())
        {
            return std::invoke(std::forward<F>(f), std::get<0>(m_storage).value);
        }
        return R::err(std::get<1>(m_storage).value);
    }

    template <typename F>
    [[nodiscard]] auto and_then(F&& f) && -> std::invoke_result_t<F, T&&>
    {
        using R = std::invoke_result_t<F, T&&>;
        if (has_value())
        {
            return std::invoke(std::forward<F>(f), std::move(std::get<0>(m_storage).value));
        }
        return R::err(std::move(std::get<1>(m_storage).value));
    }
    /// @}

    /// @brief Passes the error to @p f and returns its `Result`. Propagates values.
    ///
    /// Mirrors `std::expected::or_else()` (C++23).
    ///
    /// @tparam F Callable: `Result<T, G> f(const E&)` or `Result<T, G> f(E&&)`.
    /// @{
    template <typename F>
    [[nodiscard]] auto or_else(F&& f) const& -> std::invoke_result_t<F, const E&>
    {
        using R = std::invoke_result_t<F, const E&>;
        if (has_error())
        {
            return std::invoke(std::forward<F>(f), std::get<1>(m_storage).value);
        }
        return R::ok(std::get<0>(m_storage).value);
    }

    template <typename F>
    [[nodiscard]] auto or_else(F&& f) && -> std::invoke_result_t<F, E&&>
    {
        using R = std::invoke_result_t<F, E&&>;
        if (has_error())
        {
            return std::invoke(std::forward<F>(f), std::move(std::get<1>(m_storage).value));
        }
        return R::ok(std::move(std::get<0>(m_storage).value));
    }
    /// @}

    // -------------------------------------------------------------------------
    // Inspection
    // -------------------------------------------------------------------------

    /// @brief Invokes @p f with the value if present. Returns the result for chaining.
    /// @tparam F Callable: `void f(const T&)`.
    /// @{
    template <typename F>
    const Result& inspect(F&& f) const&
    {
        if (has_value())
        {
            std::invoke(std::forward<F>(f), std::get<0>(m_storage).value);
        }
        return *this;
    }

    template <typename F>
    Result&& inspect(F&& f) &&
    {
        if (has_value())
        {
            std::invoke(std::forward<F>(f), std::get<0>(m_storage).value);
        }
        return std::move(*this);
    }
    /// @}

    /// @brief Invokes @p f with the error if present. Returns the result for chaining.
    /// @tparam F Callable: `void f(const E&)`.
    /// @{
    template <typename F>
    const Result& inspect_error(F&& f) const&
    {
        if (has_error())
        {
            std::invoke(std::forward<F>(f), std::get<1>(m_storage).value);
        }
        return *this;
    }

    template <typename F>
    Result&& inspect_error(F&& f) &&
    {
        if (has_error())
        {
            std::invoke(std::forward<F>(f), std::get<1>(m_storage).value);
        }
        return std::move(*this);
    }
    /// @}

    // -------------------------------------------------------------------------
    // Comparison
    // -------------------------------------------------------------------------

    /// @brief Equality operator; true if both hold the same state and equal contents.
    [[nodiscard]] bool operator==(const Result& other) const
        noexcept(noexcept(std::declval<T>() == std::declval<T>()) &&
                 noexcept(std::declval<E>() == std::declval<E>()))
    {
        if (m_storage.index() != other.m_storage.index()) { return false; }
        if (has_value())
        {
            return std::get<0>(m_storage).value == std::get<0>(other.m_storage).value;
        }
        return std::get<1>(m_storage).value == std::get<1>(other.m_storage).value;
    }

    /// @brief Inequality operator; true if the results are not equal.
    [[nodiscard]] bool operator!=(const Result& other) const
        noexcept(noexcept(*this == other))
    {
        return !(*this == other);
    }

private:
    Storage m_storage;

    explicit Result(detail::OkHolder<T> h)  : m_storage(std::move(h)) {}
    explicit Result(detail::ErrHolder<E> h) : m_storage(std::move(h)) {}
};

} // namespace types

} // namespace foundry
