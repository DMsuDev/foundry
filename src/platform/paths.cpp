// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/platform/paths.h"

#include <stdexcept>
#include <vector>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#endif

namespace foundry::platform {

std::filesystem::path working_directory()
{
    std::error_code ec;
    auto path = std::filesystem::current_path(ec);
    if (ec) {
        return {};
    }
    return path;
}

std::filesystem::path executable_path()
{
#if defined(_WIN32)

    // Use a dynamic buffer to safely support long paths (> 260 characters)
    std::vector<wchar_t> buffer(MAX_PATH);
    while (true) {
        DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (size == 0) {
            throw std::runtime_error("Failed to retrieve executable path (Windows Error)");
        }
        if (size < buffer.size()) {
            return std::filesystem::path(buffer.data());
        }
        // Buffer was too small, double the size and try again
        buffer.resize(buffer.size() * 2);
    }

#elif defined(__linux__)

    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (len <= 0) {
        throw std::runtime_error("Failed to read /proc/self/exe link");
    }

    buffer[len] = '\0';
    return std::filesystem::path(buffer);

#elif defined(__APPLE__)

    uint32_t size = 0;
    // First call determines the required buffer size
    _NSGetExecutablePath(nullptr, &size);

    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
        std::error_code ec;
        auto absolute_path = std::filesystem::canonical(buffer.data(), ec);
        return ec ? std::filesystem::path(buffer.data()) : absolute_path;
    }
    throw std::runtime_error("Failed to retrieve executable path (macOS Error)");

#else
    #error "Unsupported platform for foundry::platform"
#endif
}

std::filesystem::path executable_directory()
{
    return executable_path().parent_path();
}

std::filesystem::path temp_directory()
{
    std::error_code ec;
    auto path = std::filesystem::temp_directory_path(ec);
    if (ec) {
        return {};
    }
    return path;
}

} // namespace foundry::platform
