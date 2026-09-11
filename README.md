# Foundry

[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Version](https://img.shields.io/github/v/release/DMsuDev/foundry?style=flat&label=Version&color=purple)](https://github.com/DMsuDev/foundry/releases/latest)
[![License MIT](https://img.shields.io/github/license/DMsuDev/foundry?style=flat&label=License&logo=open-source-initiative&logoColor=white)](https://github.com/DMsuDev/foundry/blob/main/LICENSE)
![Platform Windows](https://img.shields.io/badge/Platform-Windows-0078D6?style=flat&logo=windows&logoColor=white)
![Platform Linux](https://img.shields.io/badge/Platform-Linux-2d2d2d?style=flat&logo=linux&logoColor=white)
![Platform macOS](https://img.shields.io/badge/Platform-macOS-000000?style=flat&logo=apple&logoColor=white)

**Foundry** is a cross-platform C++ utility library that collects small, self-contained tools commonly needed across projects, so they don't have to be rewritten each time. It focuses on a handful of dependency-light building blocks with clear, minimal APIs.

Built with **C++17** as its baseline standard, Foundry targets **Linux, macOS, and Windows**, and each utility can be adopted on its own without pulling in the rest of the library.

## Features

### 🔹 Scope Guard

Runs a piece of cleanup code when the current scope ends, whether it exits normally, early, or through an exception. Use `ScopeGuard` directly, or the `FOUNDRY_ON_SCOPE_EXIT` / `FOUNDRY_ON_SCOPE_EXIT_NAMED` macros to declare the cleanup inline at the point where the resource is acquired.

### 🔹 Flags

A type-safe wrapper for enum-based bitmasks. `Flags<E>` lets you combine, check, and clear options through a proper type instead of raw integers, and `FOUNDRY_DECLARE_FLAGS` generates the operators needed to use an enum with it.

## CMake integration

Foundry can be integrated into a CMake project using either `FetchContent` or `add_subdirectory()`.

### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  foundry
  GIT_REPOSITORY https://github.com/DMsuDev/foundry.git
  GIT_TAG v0.1.0
)

FetchContent_MakeAvailable(foundry)
target_link_libraries(my_app PRIVATE Foundry::Foundry)
```

> For reproducible builds, pin `GIT_TAG` to a specific release tag or commit hash.

### Subdirectory

```cmake
add_subdirectory(vendor/foundry)
target_link_libraries(my_app PRIVATE Foundry::Foundry)
```

## Examples

| Example                 | Source                                                                               | Description                                           |
| ----------------------- | ------------------------------------------------------------------------------------ | ----------------------------------------------------- |
| **scope_guard_example** | [`examples/memory/scope_guard_example.cpp`](examples/memory/scope_guard_example.cpp) | Basic usage of `ScopeGuard` and `make_scope_guard()`. |
| **flags_example**       | [`examples/types/flags_example.cpp`](examples/types/flags_example.cpp)               | Type-safe bitmask operations with `Flags<E>`.         |

## License

**Foundry** is licensed under the **MIT License**.<br>
See [`LICENSE`](LICENSE) for more information.
