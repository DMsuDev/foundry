// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#pragma once

/**
 * @file visibility.h
 * @brief Shared library symbol visibility utilities.
 *
 * Provides a portable abstraction for controlling symbol visibility when
 * building or consuming Foundry as a shared library.
 */

// ============================================================================
// Symbol Export
// ============================================================================

/// @def FOUNDRY_EXPORT
/// @brief Declares a public symbol export or import for shared library builds.
#if defined(FOUNDRY_SHARED_LIBRARY)

    // ---- Windows / Cygwin ------------------------------------------------
    #if defined(_WIN32) || defined(__CYGWIN__)
        #if defined(FOUNDRY_EXPORT_SYMBOLS)
            // Export public symbols while building the DLL.
            #define FOUNDRY_EXPORT __declspec(dllexport)
        #else
            // Import public symbols when consuming the DLL.
            #define FOUNDRY_EXPORT __declspec(dllimport)
        #endif

    // ---- GCC/Clang visible platforms (Linux, macOS, etc.) ----------------
    #elif defined(__linux__) || defined(__APPLE__) && defined(__MACH__)
        #define FOUNDRY_EXPORT __attribute__((visibility("default")))

    // ---- Unknown platform ------------------------------------------------
    #else
        #error "[Foundry] Unsupported platform for shared library symbol visibility."
    #endif
#else
    // No import/export attributes are required for static libraries.
    #define FOUNDRY_EXPORT
#endif
