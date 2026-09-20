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
///
/// @par Basic usage
/// @code
/// Result<int, std::string> parse_int(std::string_view s)
/// {
///     try   { return Result<int, std::string>::ok(std::stoi(std::string(s))); }
///     catch (...) { return Result<int, std::string>::err("not a valid integer"); }
/// }
///
/// auto result = parse_int("42");
/// if (result) { use(*result); }
/// else        { log(result.error()); }
/// @endcode
///
/// @par Monadic chaining
/// @code
/// parse_int("10")
///     .map([](int n) { return n * 2; })
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

/**
 * @brief Exception thrown when accessing a Result in the wrong state.
 *
 * Thrown by `value()` on an error result, or `error()` on a success result.
 */
class bad_result_access : public std::logic_error
{
public:
    explicit bad_result_access(const char* what) : std::logic_error(what) {}
};

// ============================================================================
// Detail
// ============================================================================

namespace detail {

/// @cond INTERNAL

// Index-tagged wrappers so std::variant<T, E> works even when T == E.
template <typename T> struct OkHolder  { T value; };
template <typename E> struct ErrHolder { E value; };

/// @endcond

} // namespace detail

namespace types {

// ============================================================================
// Result<T, E>
// ============================================================================

/**
 * @brief Holds either a success value (`T`) or an error value (`E`).
 *
 * @tparam T Success type. Must be move-constructible. Must not be `void`.
 * @tparam E Error type.  Must be move-constructible. Must not be `void`.
 *
 * Use `Result<std::monostate, E>` for operations that succeed with no value.
 * `T` and `E` may be the same type.
 */
template <typename T, typename E>
class Result
{
    static_assert(!std::is_void_v<T>, "T must not be void. Use std::monostate instead.");
    static_assert(!std::is_void_v<E>, "E must not be void. Use std::monostate instead.");

    using Storage = std::variant<detail::OkHolder<T>, detail::ErrHolder<E>>;

public:
    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------

    /// @brief Constructs a successful result holding @p value.
    template <typename U = T, std::enable_if_t<std::is_constructible_v<T, U&&>, int> = 0>
    [[nodiscard]] static Result ok(U&& value)
    {
        Result r;
        r.m_storage.template emplace<0>(detail::OkHolder<T>{ T(std::forward<U>(value)) });
        return r;
    }

    /// @brief Constructs an error result holding @p error.
    template <typename G = E, std::enable_if_t<std::is_constructible_v<E, G&&>, int> = 0>
    [[nodiscard]] static Result err(G&& error)
    {
        Result r;
        r.m_storage.template emplace<1>(detail::ErrHolder<E>{ E(std::forward<G>(error)) });
        return r;
    }

    // -------------------------------------------------------------------------
    // State query
    // -------------------------------------------------------------------------

    /// @brief Returns true if this result holds a value.
    [[nodiscard]] bool has_value() const noexcept { return m_storage.index() == 0; }

    /// @brief Returns true if this result holds an error.
    [[nodiscard]] bool is_error() const noexcept { return m_storage.index() == 1; }

    /// @brief Contextual bool conversion. Returns true if has_value().
    explicit operator bool() const noexcept { return has_value(); }

    // -------------------------------------------------------------------------
    // Value access
    // -------------------------------------------------------------------------

    /**
     * @brief Returns a reference to the contained value.
     * @throws bad_result_access if this result holds an error.
     */
    [[nodiscard]] T& value() &
    {
        if (!has_value()) throw bad_result_access("called value() on an error Result");
        return std::get<0>(m_storage).value;
    }

    /**
     * @brief Returns a const reference to the contained value.
     * @throws bad_result_access if this result holds an error.
     */
    [[nodiscard]] const T& value() const&
    {
        if (!has_value()) throw bad_result_access("called value() on an error Result");
        return std::get<0>(m_storage).value;
    }

    /**
     * @brief Returns an rvalue reference to the contained value.
     * @throws bad_result_access if this result holds an error.
     */
    [[nodiscard]] T&& value() &&
    {
        if (!has_value()) throw bad_result_access("called value() on an error Result");
        return std::move(std::get<0>(m_storage).value);
    }

    /**
     * @brief Dereference operator. Equivalent to value().
     * @throws bad_result_access if this result holds an error.
     */
    [[nodiscard]] T&       operator*()  &      { return value(); }
    [[nodiscard]] const T& operator*()  const& { return value(); }
    [[nodiscard]] T&&      operator*()  &&     { return std::move(*this).value(); }

    /**
     * @brief Arrow operator. Equivalent to &value().
     * @throws bad_result_access if this result holds an error.
     */
    [[nodiscard]] T*       operator->()       { return &value(); }
    [[nodiscard]] const T* operator->() const { return &value(); }

    /**
     * @brief Returns the value, or @p fallback if this holds an error.
     */
    template <typename U>
    [[nodiscard]] T value_or(U&& fallback) const&
    {
        return has_value() ? std::get<0>(m_storage).value
                           : static_cast<T>(std::forward<U>(fallback));
    }

    /**
     * @brief Returns the value (moved), or @p fallback if this holds an error.
     */
    template <typename U>
    [[nodiscard]] T value_or(U&& fallback) &&
    {
        return has_value() ? std::move(std::get<0>(m_storage).value)
                           : static_cast<T>(std::forward<U>(fallback));
    }

    // -------------------------------------------------------------------------
    // Error access
    // -------------------------------------------------------------------------

    /**
     * @brief Returns a reference to the contained error.
     * @throws bad_result_access if this result holds a value.
     */
    [[nodiscard]] E& error() &
    {
        if (!is_error()) throw bad_result_access("called error() on a success Result");
        return std::get<1>(m_storage).value;
    }

    /**
     * @brief Returns a const reference to the contained error.
     * @throws bad_result_access if this result holds a value.
     */
    [[nodiscard]] const E& error() const&
    {
        if (!is_error()) throw bad_result_access("called error() on a success Result");
        return std::get<1>(m_storage).value;
    }

    /**
     * @brief Returns an rvalue reference to the contained error.
     * @throws bad_result_access if this result holds a value.
     */
    [[nodiscard]] E&& error() &&
    {
        if (!is_error()) throw bad_result_access("called error() on a success Result");
        return std::move(std::get<1>(m_storage).value);
    }

    // -------------------------------------------------------------------------
    // Monadic operations
    // -------------------------------------------------------------------------

    /**
     * @brief Applies @p f to the value and returns a new `Result<U, E>`.
     *
     * If this holds an error, forwards it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `U f(const T&)`.
     */
    template <typename F>
    [[nodiscard]] auto map(F&& f) const& -> Result<std::invoke_result_t<F, const T&>, E>
    {
        using U = std::invoke_result_t<F, const T&>;
        if (has_value())
            return Result<U, E>::ok(std::invoke(std::forward<F>(f), std::get<0>(m_storage).value));
        return Result<U, E>::err(std::get<1>(m_storage).value);
    }

    /**
     * @brief Applies @p f to the value (moved) and returns a new `Result<U, E>`.
     *
     * If this holds an error, forwards it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `U f(T&&)`.
     */
    template <typename F>
    [[nodiscard]] auto map(F&& f) && -> Result<std::invoke_result_t<F, T&&>, E>
    {
        using U = std::invoke_result_t<F, T&&>;
        if (has_value())
            return Result<U, E>::ok(std::invoke(std::forward<F>(f), std::move(std::get<0>(m_storage).value)));
        return Result<U, E>::err(std::move(std::get<1>(m_storage).value));
    }

    /**
     * @brief Applies @p f to the error and returns a new `Result<T, G>`.
     *
     * If this holds a value, forwards it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `G f(const E&)`.
     */
    template <typename F>
    [[nodiscard]] auto map_error(F&& f) const& -> Result<T, std::invoke_result_t<F, const E&>>
    {
        using G = std::invoke_result_t<F, const E&>;
        if (is_error())
            return Result<T, G>::err(std::invoke(std::forward<F>(f), std::get<1>(m_storage).value));
        return Result<T, G>::ok(std::get<0>(m_storage).value);
    }

    /**
     * @brief Applies @p f to the error (moved) and returns a new `Result<T, G>`.
     *
     * If this holds a value, forwards it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `G f(E&&)`.
     */
    template <typename F>
    [[nodiscard]] auto map_error(F&& f) && -> Result<T, std::invoke_result_t<F, E&&>>
    {
        using G = std::invoke_result_t<F, E&&>;
        if (is_error())
            return Result<T, G>::err(std::invoke(std::forward<F>(f), std::move(std::get<1>(m_storage).value)));
        return Result<T, G>::ok(std::move(std::get<0>(m_storage).value));
    }

    /**
     * @brief Passes the value to @p f and returns its `Result`. Propagates errors.
     *
     * If this holds an error, returns it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `Result<U, E> f(const T&)`.
     */
    template <typename F>
    [[nodiscard]] auto and_then(F&& f) const& -> std::invoke_result_t<F, const T&>
    {
        using R = std::invoke_result_t<F, const T&>;
        if (has_value())
            return std::invoke(std::forward<F>(f), std::get<0>(m_storage).value);
        return R::err(std::get<1>(m_storage).value);
    }

    /**
     * @brief Passes the value (moved) to @p f and returns its `Result`. Propagates errors.
     *
     * If this holds an error, returns it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `Result<U, E> f(T&&)`.
     */
    template <typename F>
    [[nodiscard]] auto and_then(F&& f) && -> std::invoke_result_t<F, T&&>
    {
        using R = std::invoke_result_t<F, T&&>;
        if (has_value())
            return std::invoke(std::forward<F>(f), std::move(std::get<0>(m_storage).value));
        return R::err(std::move(std::get<1>(m_storage).value));
    }

    /**
     * @brief Passes the error to @p f and returns its `Result`. Propagates values.
     *
     * If this holds a value, returns it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `Result<T, G> f(const E&)`.
     */
    template <typename F>
    [[nodiscard]] auto or_else(F&& f) const& -> std::invoke_result_t<F, const E&>
    {
        using R = std::invoke_result_t<F, const E&>;
        if (is_error())
            return std::invoke(std::forward<F>(f), std::get<1>(m_storage).value);
        return R::ok(std::get<0>(m_storage).value);
    }

    /**
     * @brief Passes the error (moved) to @p f and returns its `Result`. Propagates values.
     *
     * If this holds a value, returns it unchanged without invoking @p f.
     *
     * @tparam F Callable with signature `Result<T, G> f(E&&)`.
     */
    template <typename F>
    [[nodiscard]] auto or_else(F&& f) && -> std::invoke_result_t<F, E&&>
    {
        using R = std::invoke_result_t<F, E&&>;
        if (is_error())
            return std::invoke(std::forward<F>(f), std::move(std::get<1>(m_storage).value));
        return R::ok(std::move(std::get<0>(m_storage).value));
    }

    // -------------------------------------------------------------------------
    // Inspection
    // -------------------------------------------------------------------------

    /**
     * @brief Invokes @p f with the value if present. Returns `*this` for chaining.
     * @tparam F Callable with signature `void f(const T&)`.
     */
    template <typename F>
    const Result& inspect(F&& f) const&
    {
        if (has_value())
            std::invoke(std::forward<F>(f), std::get<0>(m_storage).value);
        return *this;
    }

    /**
     * @brief Invokes @p f with the error if present. Returns `*this` for chaining.
     * @tparam F Callable with signature `void f(const E&)`.
     */
    template <typename F>
    const Result& inspect_error(F&& f) const&
    {
        if (is_error())
            std::invoke(std::forward<F>(f), std::get<1>(m_storage).value);
        return *this;
    }

    // -------------------------------------------------------------------------
    // Comparison
    // -------------------------------------------------------------------------

    [[nodiscard]] bool operator==(const Result& other) const
        noexcept(noexcept(std::declval<T>() == std::declval<T>()) &&
                 noexcept(std::declval<E>() == std::declval<E>()))
    {
        if (m_storage.index() != other.m_storage.index()) return false;
        if (has_value())
            return std::get<0>(m_storage).value == std::get<0>(other.m_storage).value;
        return std::get<1>(m_storage).value == std::get<1>(other.m_storage).value;
    }

    [[nodiscard]] bool operator!=(const Result& other) const
        noexcept(noexcept(*this == other))
    {
        return !(*this == other);
    }

private:
    Storage m_storage;

    Result() = default;
};

} // namespace types

} // namespace foundry
