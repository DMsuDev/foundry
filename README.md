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

- **Scope Guard** - Cleanup code guaranteed to run on scope exit, early return, or exception. [`scope_guard_example.cpp`](examples/memory/scope_guard_example.cpp)
- **Run Once / Run At Exit** - One-shot execution per call site and lambda-based `atexit` registration with LIFO ordering. [`run_once_example.cpp`](examples/memory/run_once_example.cpp)
- **Flags** - Type-safe bitmask container for `enum class` values with compile-time mixing prevention. [`flags_example.cpp`](examples/types/flags_example.cpp)
- **Result** - Exceptionless, type-safe `Result<T, E>` monad for flexible error handling and chaining. [`result_example.cpp`](examples/types/result_example.cpp)
- **Semantic Version** - Lightweight parser and representation for Semantic Versioning. [`semver_example.cpp`](examples/types/semver_example.cpp)
- **Platform Paths** - Runtime resolution of working directory, executable path, and temp directory as `std::filesystem::path`. [`paths_example.cpp`](examples/platform/paths_example.cpp)

## CMake integration

Foundry can be integrated into a CMake project using either `FetchContent` or `add_subdirectory()`.

### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  foundry
  GIT_REPOSITORY https://github.com/DMsuDev/foundry.git
  GIT_TAG        # pin to a release tag or commit hash
)

FetchContent_MakeAvailable(foundry)
target_link_libraries(my_app PRIVATE Foundry::Foundry)
```

### Subdirectory

```cmake
add_subdirectory(vendor/foundry)
target_link_libraries(my_app PRIVATE Foundry::Foundry)
```

## License

**Foundry** is licensed under the **MIT License**.<br>
See [`LICENSE`](LICENSE) for more information.
