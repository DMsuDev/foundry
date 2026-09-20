// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/platform/paths.h"

#include <vector>
#include <system_error>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#else
    #error "Unsupported platform for foundry::platform"
#endif

namespace foundry::platform {

// ============================================================================
// Working directory
// ============================================================================

std::filesystem::path working_directory(std::error_code& ec) noexcept
{
    return std::filesystem::current_path(ec);
}

std::filesystem::path working_directory()
{
    std::error_code ec;
    auto path = working_directory(ec);
    if (ec) {
        throw std::system_error(ec, "Failed to retrieve working directory");
    }
    return path;
}

// ============================================================================
// Executable path
// ============================================================================


std::filesystem::path executable_path(std::error_code& ec) noexcept
{
#if defined(_WIN32)

    // Use a dynamic buffer to safely support long paths (> 260 characters)
    std::vector<wchar_t> buffer(MAX_PATH);
    while (true) {
        DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (size == 0)
        {
            ec = std::error_code(static_cast<int>(GetLastError()), std::system_category());
            return {};
        }
        if (size < buffer.size())
        {
            ec.clear();
            return std::filesystem::path(buffer.data(), buffer.data() + size);
        }
        // Buffer was too small, double the size and try again
        buffer.resize(buffer.size() * 2);
    }

#elif defined(__linux__)

    std::vector<char> buffer(256);

    while (true)
    {
        const ssize_t size = readlink("/proc/self/exe", buffer.data(), buffer.size());

        if (size < 0)
        {
            ec = std::error_code(errno, std::generic_category());
            return {};
        }

        if (static_cast<std::size_t>(size) < buffer.size())
        {
            ec.clear();
            return std::filesystem::path(buffer.data(), buffer.data() + size);
        }

        buffer.resize(buffer.size() * 2);
    }

#elif defined(__APPLE__)

    uint32_t size = 0;
    // First call determines the required buffer size
    _NSGetExecutablePath(nullptr, &size);

    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) != 0)
    {
        ec = std::make_error_code(std::errc::io_error);
        return {};
    }

    ec.clear();
    // _NSGetExecutablePath() can return a non-canonical path.
    return std::filesystem::path(buffer.data());

#else
    static_assert(false, "Unsupported platform for foundry::platform");
#endif
}

std::filesystem::path executable_path()
{
    std::error_code ec;
    auto path = executable_path(ec);
    if (ec) {
        throw std::system_error(ec, "Failed to retrieve executable path");
    }
    return path;
}

// ============================================================================
// Executable directory
// ============================================================================

std::filesystem::path executable_directory(std::error_code& ec) noexcept
{
    auto path = executable_path(ec);
    if (ec) return {};
    return path.parent_path();
}

std::filesystem::path executable_directory()
{
    std::error_code ec;
    auto path = executable_directory(ec);
    if (ec) {
        throw std::system_error(ec, "Failed to retrieve executable directory");
    }
    return path;
}

// ============================================================================
// Temporary directory
// ============================================================================

std::filesystem::path temp_directory(std::error_code& ec) noexcept
{
    return std::filesystem::temp_directory_path(ec);
}

std::filesystem::path temp_directory()
{
    std::error_code ec;
    auto path = temp_directory(ec);
    if (ec) {
        throw std::system_error(ec, "Failed to retrieve temporary directory");
    }
    return path;
}

} // namespace foundry::platform
