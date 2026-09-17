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

Runs a piece of cleanup code when the current scope ends, whether it exits normally, early, or through an exception. Use `ScopeGuard` directly, or the `FOUNDRY_SCOPE_EXIT` / `FOUNDRY_SCOPE_EXIT_NAMED` macros to declare the cleanup inline at the point where the resource is acquired. A named guard can be dismissed if the operation succeeds.

<details>
<summary>Example</summary>

```cpp
FILE* file = fopen("data.txt", "r");

FOUNDRY_SCOPE_EXIT {
    fclose(file);  // always runs, even on early return or exception
};

// Dismissible: cleanup fires only if do_work() throws
db.begin_transaction();

FOUNDRY_SCOPE_EXIT_NAMED(rollback) {
    db.rollback();
};

do_work();          // if this throws, rollback fires
rollback.dismiss(); // success: skip the rollback
db.commit();
```

</details>

---

### 🔹 Run Once / Run At Exit

`FOUNDRY_RUN_ONCE` executes a block exactly once per call site, no matter how many times that line is reached at runtime. Thread-safe via C++11 static initialization guarantees.

`FOUNDRY_RUN_AT_EXIT` registers a block to run when the program exits, during static storage destruction. Equivalent to `std::atexit` but with lambda syntax. Multiple registrations fire in reverse order of declaration (LIFO).

<details>
<summary>Example</summary>

```cpp
void init_subsystem() {
    FOUNDRY_RUN_ONCE {
        AudioSystem::global_init();
    };
}

FOUNDRY_RUN_AT_EXIT {
    LogSystem::flush_all();  // runs after main() returns
};
```

> **Note:** Always capture by value (`[=]`) inside `FOUNDRY_RUN_AT_EXIT` blocks. Local variables captured by reference will be dangling at shutdown time.

</details>

---

### 🔹 Flags

A type-safe wrapper for enum-based bitmasks. `Flags<E>` lets you combine, test, and mutate a set of options through a proper type instead of raw integers, preventing accidental mixing of unrelated enums at compile time. Supports both 32-bit and 64-bit underlying types.

Place `FOUNDRY_DECLARE_FLAGS(YourEnum)` after the enum definition to generate the operators needed to use it with `Flags<E>`.

<details>
<summary>Example</summary>

<br>

```cpp
enum class Permission : std::uint32_t {
    None    = 0,
    Read    = FOUNDRY_BIT(0),
    Write   = FOUNDRY_BIT(1),
    Execute = FOUNDRY_BIT(2),
};

FOUNDRY_DECLARE_FLAGS(Permission)

Flags<Permission> perms = Permission::Read | Permission::Write;

perms.has(Permission::Read);  // true
perms.set(Permission::Execute);
perms.unset(Permission::Write);
perms.toggle(Permission::Read);
perms.any();   // true if at least one bit is set
perms.clear();
```

</details>

---

### 🔹 Platform Paths

Queries common filesystem locations at runtime: the working directory, the executable's path and containing directory, and the system temporary directory. All functions return `std::filesystem::path` and handle errors without throwing, except for `executable_path()` and `executable_directory()`, which throw `std::runtime_error` if the path cannot be retrieved.

<details>
<summary>Example</summary>

```cpp
#include "foundry/platform/paths.h"

// Working directory — returns empty path on failure
auto cwd = foundry::platform::working_directory();

// Executable location — throws std::runtime_error on failure
auto exe = foundry::platform::executable_path();
auto dir = foundry::platform::executable_directory();

// System temp directory — returns empty path on failure
auto tmp = foundry::platform::temp_directory();
```

</details>

## CMake integration

Foundry can be integrated into a CMake project using either `FetchContent` or `add_subdirectory()`.

### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  foundry
  GIT_REPOSITORY https://github.com/DMsuDev/foundry.git
  GIT_TAG v0.2.1
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

| Example                 | Source                                                                               | Description                                                           |
| ----------------------- | ------------------------------------------------------------------------------------ | --------------------------------------------------------------------- |
| **scope_guard_example** | [`examples/memory/scope_guard_example.cpp`](examples/memory/scope_guard_example.cpp) | Basic usage of `ScopeGuard` and `make_scope_guard()`.                 |
| **run_once_example**    | [`examples/memory/run_once_example.cpp`](examples/memory/run_once_example.cpp)       | One-shot execution with `FOUNDRY_RUN_ONCE` and `FOUNDRY_RUN_AT_EXIT`. |
| **flags_example**       | [`examples/types/flags_example.cpp`](examples/types/flags_example.cpp)               | Type-safe bitmask operations with `Flags<E>`.                         |
| **paths_example**       | [`examples/platform/paths_example.cpp`](examples/platform/paths_example.cpp)         | Runtime path queries: working dir, executable, and temp directory.    |

## License

**Foundry** is licensed under the **MIT License**.<br>
See [`LICENSE`](LICENSE) for more information.
