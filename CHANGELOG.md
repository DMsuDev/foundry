# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Conventional Commits](https://www.conventionalcommits.org/) and this project adheres to [Semantic Versioning](https://semver.org/).

## [0.3.0] - 2026-09-17

### 🚀 Features

- Add visibility macros header for shared library exports ([e2804bb](https://github.com/DMsuDev/foundry/commit/e2804bb0cdf1cbb41974ae4e4359fd127f5f9bd1))

- Add platform paths utilities, example, and unit tests ([2bbe110](https://github.com/DMsuDev/foundry/commit/2bbe110a7d81b5d3ca97410e84b70a953fcd5e92))

  - Add cross-platform runtime path utilities in foundry::platform namespace:
    working_directory(), executable_path(), executable_directory(), and temp_directory()
  - Implement Windows, Linux, and macOS OS-specific path resolution

### 🐛 Bug Fixes

- Remove redundant release existence check ([9027d24](https://github.com/DMsuDev/foundry/commit/9027d244ddd11ec5763a2e7cfd2a1c95cc87861b))

- Enforce C++17 feature in PUBLIC scope for Foundry target ([e498528](https://github.com/DMsuDev/foundry/commit/e498528d460c499c4ea6c3c934c8520696b93784))

### 🛠️ Build System

- Convert Foundry from interface to static/shared target ([e9feacf](https://github.com/DMsuDev/foundry/commit/e9feacf0ca58fc6efaa2e7558d05511546491929))

  - Change Foundry target from INTERFACE to standard library using globbed sources
  - Add BUILD_SHARED_LIBS option (default OFF)
  - Update target_include_directories scope from INTERFACE to PUBLIC
  - Add export/import macro definitions and default symbol visibility for shared builds

## [0.2.1] - 2026-09-13

### 🛠️ Build System

- Restructure test architecture and add consumer integration tests ([5895c0c](https://github.com/DMsuDev/foundry/commit/5895c0cc8fcff2682ae555d54b5e6cc20ba3ea0e))

  - Restructure `tests/` into `unit/` and CMake integration subdirectories
  - Move GoogleTest setup logic into `tests/unit/CMakeLists.txt`
  - Add consumer integration tests for `add_subdirectory` and `FetchContent`
  - Introduce `FOUNDRY_INSTALL_TESTS_CLEANUP` option and validation logic

- Remove redundant INCLUDES DESTINATION from target install ([5022249](https://github.com/DMsuDev/foundry/commit/5022249ad1c663cd3737f95fb7104d8278323f42))

### 🔧 Maintenance

- Generate version header and check release existence in workflow ([ebba698](https://github.com/DMsuDev/foundry/commit/ebba6986fad4957e4a556c407d1336ba5e4a2577))

- Add macOS matrix runner and enable consumer integration tests ([c40c031](https://github.com/DMsuDev/foundry/commit/c40c031bb9c3d2af7a2e34b2631e452b82f9457d))

  - Include `cmake/**` in workflow path triggers for `push` and `pull_request`
  - Add `macos-latest` to matrix OS runners
  - Enable `FOUNDRY_CONSUMER_TESTS` and `FOUNDRY_CONSUMER_TESTS_CLEANUP` in build steps

## [0.2.0] - 2026-09-12

### 💥 Breaking Changes

- Redesign scope guard macros to use block syntax ([a6b9862](https://github.com/DMsuDev/foundry/commit/a6b9862edb9d5910992f5157df24749724dd0801))

> ⚠️ `FOUNDRY_SCOPE_EXIT` and `FOUNDRY_SCOPE_EXIT_NAMED` now expect a block `{ ... };` instead of taking code as a macro argument.

- Updated `FOUNDRY_SCOPE_EXIT` and `FOUNDRY_SCOPE_EXIT_NAMED` to follow block syntax
- Added `[[maybe_unused]]` to prevent compiler warnings
- Tightened `ScopeGuard` type constraint with `std::is_invocable_r_v`

To migrate your code:
- Replace `FOUNDRY_SCOPE_EXIT(code);` with `FOUNDRY_SCOPE_EXIT { code; };`
- Replace `FOUNDRY_SCOPE_EXIT_NAMED(name, code);` with `FOUNDRY_SCOPE_EXIT_NAMED(name) { code; };`

### 🚀 Features

- Add FOUNDRY_RUN_ONCE and FOUNDRY_RUN_AT_EXIT utilities ([2af0855](https://github.com/DMsuDev/foundry/commit/2af0855d787d17425ab4801f0ca5d266e6692fd1))

  - Core utilities: Introduced thread-safe `RunOnce` and `RunAtExit` wrappers with block-syntax macros (`FOUNDRY_RUN_ONCE`, `FOUNDRY_RUN_AT_EXIT`).
  - Test coverage: Added unit tests covering thread safety, conditional execution, and LIFO destruction ordering.
  - Documentation & Examples: Included `run_once_example` showcasing initialization and shutdown usage patterns.

### 🐛 Bug Fixes

- Prevent installing header template files ([98e334c](https://github.com/DMsuDev/foundry/commit/98e334cf7fc7a1584c2c1ecca8931faa7a533cf4))

- Wrap enable_flags specialization in detail namespace ([6747cce](https://github.com/DMsuDev/foundry/commit/6747cce8b3ab99b977eb566c8c61ba2b9459f789))

  - Wrap `enable_flags<E>` template specialization in `foundry::types::detail` namespace within `FOUNDRY_DECLARE_FLAGS`
  - Fixes compilation error when macro is invoked from inside a custom namespace

### 🔧 Maintenance

- Add CI build workflow and CodeQL static analysis ([1f8eb0d](https://github.com/DMsuDev/foundry/commit/1f8eb0d498048821e301110e72eb10fa6db4e8e3))

- Fix Windows line continuation syntax in CI workflow ([ea9ae40](https://github.com/DMsuDev/foundry/commit/ea9ae409a207617a10721236919dc2073c3d0e1c))

## [0.1.1] - 2026-09-11

### 💥 Breaking Changes

- Rename scope exit macros to FOUNDRY_SCOPE_EXIT ([215c418](https://github.com/DMsuDev/foundry/commit/215c418b1c7c2725666f5a5b3fb91881ba97ca45))

> ⚠️ The scope exit macros have been renamed to simplify the API and prevent name collision issues with multiple macro expansions on the same line.

- Renamed `FOUNDRY_ON_SCOPE_EXIT` to `FOUNDRY_SCOPE_EXIT`
- Renamed `FOUNDRY_ON_SCOPE_EXIT_NAMED` to `FOUNDRY_SCOPE_EXIT_NAMED`
- Updated macro expansion to use `__COUNTER__` instead of `__LINE__` for unique identifier generation
- Cleaned up usage syntax (passing statements directly without surrounding `{}`)

To migrate your code:
- Replace all occurrences of `FOUNDRY_ON_SCOPE_EXIT({ code });` with `FOUNDRY_SCOPE_EXIT(code;)`
- Replace all occurrences of `FOUNDRY_ON_SCOPE_EXIT_NAMED(name, { code });` with `FOUNDRY_SCOPE_EXIT_NAMED(name, { code; })`

## [0.1.0] - 2026-09-10

### 🏗️ Project Setup

- Initialize project scaffolding ([c688c1b](https://github.com/DMsuDev/foundry/commit/c688c1b03161c1ac597f7e3f127581b289f884ff))

  - Add project configuration files (.editorconfig, .gitattributes, .gitignore)
  - Configure CMake build environment, code style, and developer tooling (.markdownlint.yaml, .pre-commit-config.yaml)
  - Add GitHub Action workflows for pre-releases, release publishing, and Dependabot
  - Add GitHub community health templates (issue templates, PR template, CODEOWNERS, FUNDING)

### 🚀 Features

- Add `ScopeGuard` utility ([9b5f1ed](https://github.com/DMsuDev/foundry/commit/9b5f1ede27569b05f872f814d0d20809483092e1))

  Add a RAII scope guard to `foundry::memory` for executing cleanup actions when leaving scope. Supports move semantics, dismissal, and exception-safe destruction, with accompanying scope macros, unit tests, and an example.

- Add `Flags<E>` and `FOUNDRY_BIT` macros ([db95e40](https://github.com/DMsuDev/foundry/commit/db95e405cb69b810a061a1bc64d06b59ac1e6bfa))

  Introduce a type-safe bitmask wrapper over scoped enums in `foundry::types` with opt-in macro support and bitwise operators.

  - Add `Flags<E>` class template and `FOUNDRY_DECLARE_FLAGS` macro.
  - Add `FOUNDRY_BIT` and `FOUNDRY_BIT64` helper macros in `foundry/types/bitmask.h`.
  - Include bitwise operators (`|`, `&`, `^`, `~`) and mutations (`set`, `unset`, `toggle`, `clear`).

<!-- generated by git-cliff -->
