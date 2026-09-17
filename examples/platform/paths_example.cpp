// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/platform/paths.h"

#include <iostream>

int main()
{
    // --- Working directory ---------------------------------------------------
    auto cwd = foundry::platform::working_directory();
    if (cwd.empty()) {
        std::cerr << "[paths] Could not determine working directory.\n";
    } else {
        std::cout << "Working directory : " << cwd << "\n";
    }

    // --- Executable path -----------------------------------------------------
    try {
        auto exe = foundry::platform::executable_path();
        std::cout << "Executable path   : " << exe << "\n";
        std::cout << "Executable dir    : " << exe.parent_path() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[paths] executable_path() failed: " << e.what() << "\n";
    }

    // --- Temp directory -------------------------------------------------------
    auto tmp = foundry::platform::temp_directory();
    if (tmp.empty()) {
        std::cerr << "[paths] Could not determine temp directory.\n";
    } else {
        std::cout << "Temp directory    : " << tmp << "\n";
    }

    return 0;
}
