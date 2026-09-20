// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

/// @file semantic_version.h
/// @brief Semantic versioning type with full SemVer 2.0.0 support.
///
/// Supports the complete SemVer 2.0.0 specification:
///   - Core version:      MAJOR.MINOR.PATCH
///   - Pre-release:       1.2.0-alpha.1
///   - Build metadata:    1.2.0+build.42
///   - Combined:          1.2.0-alpha.1+build.42

namespace foundry::types {

// ============================================================================
// SemanticVersion
// ============================================================================

/**
 * @brief Semantic Versioning 2.0.0 container.
 *
 * Preserves build metadata for serialization, but ignores it during version
 * comparisons as specified by SemVer 2.0.0.
 */
class SemanticVersion
{
public:
    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------

    /**
     * @brief Constructs a version from individual components.
     *
     * @note The pre-release and build strings are expected to follow the
     *       SemVer 2.0.0 identifier grammar. Use parse() when validating
     *       untrusted or external input.
     */
    SemanticVersion(
        std::uint32_t major, std::uint32_t minor, std::uint32_t patch,
        std::string_view prerelease = {}, std::string_view build = {})
        : m_major(major) , m_minor(minor) , m_patch(patch)
        , m_prerelease(prerelease) , m_build(build) {}

    // -------------------------------------------------------------------------
    // Parsing
    // -------------------------------------------------------------------------

    /// @brief Parses a SemVer 2.0.0 string (e.g., "1.2.3-alpha.1+build.42").
    /// @return Parsed version, or std::nullopt if the string is invalid.
    [[nodiscard]] static std::optional<SemanticVersion> parse(std::string_view str);

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------

    [[nodiscard]] constexpr std::uint32_t major() const noexcept { return m_major; }
    [[nodiscard]] constexpr std::uint32_t minor() const noexcept { return m_minor; }
    [[nodiscard]] constexpr std::uint32_t patch() const noexcept { return m_patch; }

    /// @brief Returns the pre-release identifier (e.g. "alpha.1"). Empty string for release builds.
    [[nodiscard]] std::string_view prerelease() const noexcept { return m_prerelease; }

    /// @brief Returns the build metadata (e.g "build.42").
    [[nodiscard]] std::string_view build() const noexcept { return m_build; }

    /// @brief Returns true if this is a pre-release version.
    [[nodiscard]] bool is_prerelease() const noexcept { return !m_prerelease.empty(); }

    /// @brief Returns true if this version contains build metadata.
    [[nodiscard]] bool has_build_metadata() const noexcept { return !m_build.empty(); }

    // -------------------------------------------------------------------------
    // Version gates
    // -------------------------------------------------------------------------

    /// @brief Checks if core version (MAJOR.MINOR.PATCH) is >= given parameters.
    [[nodiscard]] bool at_least(
        std::uint32_t major,
        std::uint32_t minor,
        std::uint32_t patch) const noexcept
    {
        return *this >= SemanticVersion{major, minor, patch};
    }

    // -------------------------------------------------------------------------
    // Serialization
    // -------------------------------------------------------------------------

    /// @brief Formats as a full SemVer string (e.g. "1.2.3-alpha.1+build.42").
    [[nodiscard]] std::string to_string(bool include_build = true) const;

    /// @brief Formats only core version ("MAJOR.MINOR.PATCH").
    [[nodiscard]] std::string core_string() const;

    // -------------------------------------------------------------------------
    // Comparison (Ignores Build Metadata)
    // -------------------------------------------------------------------------

    [[nodiscard]] bool operator==(const SemanticVersion& other) const noexcept;
    [[nodiscard]] bool operator!=(const SemanticVersion& other) const noexcept;
    [[nodiscard]] bool operator< (const SemanticVersion& other) const noexcept;
    [[nodiscard]] bool operator<=(const SemanticVersion& other) const noexcept;
    [[nodiscard]] bool operator> (const SemanticVersion& other) const noexcept;
    [[nodiscard]] bool operator>=(const SemanticVersion& other) const noexcept;

private:
    std::uint32_t m_major{0}, m_minor{0}, m_patch{0};
    std::string m_prerelease;
    std::string m_build;

    // -------------------------------------------------------------------------
    // Parsing helpers
    // -------------------------------------------------------------------------

    /// @brief Parses an unsigned decimal integer component of the version.
    [[nodiscard]] static bool parse_component(std::string_view str, std::size_t& pos, std::uint32_t& value) noexcept;

    /**
     * @brief Validates a dot-separated SemVer identifier string.
     *
     * @param value Identifier string.
     * @param prerelease Whether the identifiers are pre-release identifiers.
     *
     * Numeric pre-release identifiers must not contain leading zeroes.
     */
    [[nodiscard]] static bool validate_identifiers(std::string_view value, bool prerelease) noexcept;

    [[nodiscard]] static int compare_prerelease(std::string_view lhs, std::string_view rhs) noexcept;
    [[nodiscard]] static int compare_identifier(std::string_view lhs, std::string_view rhs) noexcept;

    /**
     * @brief Returns the next dot-separated identifier.
     *
     * @param value Identifier string.
     * @param pos Current position. Updated after reading the identifier.
     */
    [[nodiscard]] static std::string_view next_identifier(std::string_view value, std::size_t& pos) noexcept;

    /// @brief Returns true if the string contains only ASCII digits.
    [[nodiscard]] static bool is_numeric(std::string_view value) noexcept;
};

// ============================================================================
// Implementation
// ============================================================================

inline std::optional<SemanticVersion> SemanticVersion::parse(std::string_view str) {
    if (str.empty()) return std::nullopt;

    std::size_t pos = 0;
    std::uint32_t major = 0, minor = 0, patch = 0;

    if (!parse_component(str, pos, major) || pos >= str.size() || str[pos++] != '.') return std::nullopt;
    if (!parse_component(str, pos, minor) || pos >= str.size() || str[pos++] != '.') return std::nullopt;
    if (!parse_component(str, pos, patch)) return std::nullopt;

    std::string_view prerelease, build;

    if (pos < str.size() && str[pos] == '-') {
        const std::size_t start = ++pos;
        while (pos < str.size() && str[pos] != '+') ++pos;
        prerelease = str.substr(start, pos - start);
        if (!validate_identifiers(prerelease, true)) return std::nullopt;
    }

    if (pos < str.size() && str[pos] == '+') {
        build = str.substr(++pos);
        if (!validate_identifiers(build, false)) return std::nullopt;
        pos = str.size();
    }

    if (pos != str.size()) return std::nullopt;
    return SemanticVersion{major, minor, patch, prerelease, build};
}

// -----------------------------------------------------------------------------

inline bool SemanticVersion::parse_component(std::string_view str, std::size_t& pos, std::uint32_t& value) noexcept {
    if (pos >= str.size() || str[pos] < '0' || str[pos] > '9') return false;

    if (str[pos] == '0') {
        ++pos;
        if (pos < str.size() && str[pos] >= '0' && str[pos] <= '9') return false; // Reject leading zeroes
        value = 0;
        return true;
    }

    std::uint64_t result = 0;

    while (pos < str.size() && str[pos] >= '0' && str[pos] <= '9') {
        result = result * 10u + static_cast<std::uint64_t>(str[pos] - '0');
        if (result > std::numeric_limits<std::uint32_t>::max()) return false;
        ++pos;
    }

    value = static_cast<std::uint32_t>(result);
    return true;
}

// -----------------------------------------------------------------------------

inline bool SemanticVersion::validate_identifiers(std::string_view value, bool prerelease) noexcept {
    if (value.empty() || value.back() == '.') return false;

    std::size_t pos = 0;
    while (pos < value.size()) {
        const std::size_t start = pos;
        while (pos < value.size() && value[pos] != '.') {
            const char c = value[pos++];
            const bool valid = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-';
            if (!valid) return false;
        }

        if (pos == start) return false; // Empty identifier ".."
        const std::string_view id = value.substr(start, pos - start);

        if (prerelease && is_numeric(id) && id.size() > 1 && id.front() == '0') return false;
        if (pos < value.size()) ++pos; // Skip '.'
    }
    return true;
}

// -----------------------------------------------------------------------------

inline bool SemanticVersion::is_numeric(std::string_view value) noexcept {
    if (value.empty()) return false;
    for (const char c : value) { if (c < '0' || c > '9') return false; }
    return true;
}

// -----------------------------------------------------------------------------

inline std::string_view SemanticVersion::next_identifier(
    std::string_view value,
    std::size_t& pos) noexcept
{
    if (pos >= value.size()) return {};

    const std::size_t start = pos;

    while (pos < value.size() && value[pos] != '.')
        ++pos;

    const std::string_view identifier = value.substr(start, pos - start);

    if (pos < value.size())
        ++pos;

    return identifier;
}

// -----------------------------------------------------------------------------

inline int SemanticVersion::compare_identifier(
    std::string_view lhs,
    std::string_view rhs) noexcept
{
    const bool lhs_numeric = is_numeric(lhs);
    const bool rhs_numeric = is_numeric(rhs);

    if (lhs_numeric && !rhs_numeric) return -1;
    if (!lhs_numeric && rhs_numeric) return  1;

    if (lhs_numeric && rhs_numeric) {
        // No leading zeroes guaranteed by parser, so length determines
        // numeric magnitude when lengths differ; lexicographic otherwise.
        if (lhs.size() != rhs.size()) return lhs.size() < rhs.size() ? -1 : 1;
    }

    const int cmp = lhs.compare(rhs);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

// -----------------------------------------------------------------------------

inline int SemanticVersion::compare_prerelease(
    std::string_view lhs,
    std::string_view rhs) noexcept
{
    if (lhs.empty() && rhs.empty()) return  0;
    if (lhs.empty())                return  1;
    if (rhs.empty())                return -1;

    std::size_t lpos = 0, rpos = 0;
    while (lpos < lhs.size() && rpos < rhs.size()) {
        const int cmp = compare_identifier(next_identifier(lhs, lpos), next_identifier(rhs, rpos));
        if (cmp != 0) return cmp;
    }

    // If all common identifiers are equal, the version with fewer identifiers
    // has lower precedence.
    if (lpos < lhs.size()) return  1;
    if (rpos < rhs.size()) return -1;

    return 0;
}

// -----------------------------------------------------------------------------
// Serialization
// -----------------------------------------------------------------------------

inline std::string SemanticVersion::core_string() const {
    std::string result;
    result.reserve(32);

    result += std::to_string(m_major);
    result += '.';
    result += std::to_string(m_minor);
    result += '.';
    result += std::to_string(m_patch);

    return result;
}

// -----------------------------------------------------------------------------

inline std::string SemanticVersion::to_string(bool include_build) const {
    std::string s = core_string();
    if (!m_prerelease.empty()) { s += '-'; s += m_prerelease; }
    if (include_build && !m_build.empty()) { s += '+'; s += m_build; }
    return s;
}

// -----------------------------------------------------------------------------
// Comparison
// -----------------------------------------------------------------------------

inline bool SemanticVersion::operator==(const SemanticVersion& o) const noexcept {
    return m_major == o.m_major
        && m_minor == o.m_minor
        && m_patch == o.m_patch
        && m_prerelease == o.m_prerelease;
}

inline bool SemanticVersion::operator!=(const SemanticVersion& o) const noexcept { return !(*this == o); }

inline bool SemanticVersion::operator<(const SemanticVersion& o) const noexcept {
    if (m_major != o.m_major) return m_major < o.m_major;
    if (m_minor != o.m_minor) return m_minor < o.m_minor;
    if (m_patch != o.m_patch) return m_patch < o.m_patch;
    return compare_prerelease(m_prerelease, o.m_prerelease) < 0;
}

inline bool SemanticVersion::operator<=(const SemanticVersion& o) const noexcept { return !(o < *this); }
inline bool SemanticVersion::operator> (const SemanticVersion& o) const noexcept { return   o < *this;  }
inline bool SemanticVersion::operator>=(const SemanticVersion& o) const noexcept { return !(*this < o); }

} // namespace foundry::types
