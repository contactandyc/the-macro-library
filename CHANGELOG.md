## 08/22/2025

**Refactor build, docs, and core APIs; add full test suite**

---

## Summary

This PR removes old changelog/config files, modernizes build & packaging, updates licensing headers, adds new documentation for each module, and introduces a comprehensive test suite with coverage support.

---

## Key Changes

### 🔧 Build & Tooling

* Removed legacy files:

    * `.changes/*`, `.changie.yaml`, `CHANGELOG.md`, `NEWS.md`, `build_install.sh`.
* Added unified **`build.sh`** with support for:

    * `build`, `install`, `coverage`, `clean`.
    * Coverage aggregation using `llvm-cov` or `gcov`.
* Updated `.gitignore` to include new build dirs (`build-unix_makefiles`, `build-cov`, `build-coverage`).
* Introduced **CMake package config** (`cmake/Config.cmake.in`) for install/export targets.
* Rewrote `CMakeLists.txt`:

    * Raised minimum version to 3.20.
    * Explicit **header-only** target with proper install/export.
    * Added `tests` subdir with `enable_testing()`.

### 📦 Container & Build Docs

* Replaced `BUILDING.md` with simplified local build instructions and modern **Dockerfile**:

    * Ubuntu base image, configurable CMake version, optional dev tools.
    * Proper non-root `dev` user.
* Removed dependency on `a-cmake-library` (was previously cloned/installed).

### 📚 Documentation

* Completely new **README.md**: concise, feature overview, quick start, algorithm notes, performance graphs.
* Added **module-specific READMEs**:

    * `README.macro_sort.md`
    * `README.macro_bsearch.md`
    * `README.macro_map.md`
    * `README.macro_cmp.md`
    * `README.macro_heap.md`
    * `README.macro_test.md`
    * `README.macro_time.md`
    * `README.macro_to.md`

### 📝 License & Metadata

* All source files updated with SPDX headers:

    * `2019–2025 Andy Curtis`
    * `2024–2025 Knode.ai — technical questions: contact Andy`
* NOTICE simplified: explicit Apache-2.0 reference.

### 🛠️ Library Internals

* `macro_map.h`:

    * Safer parent/color packing using `uintptr_t`.
    * Added `STATIC_ASSERT` for alignment.
    * Guarded rotate functions to handle `root==NULL`.
* `macro_bsearch.h` / `macro_bsearch_code.h`:

    * Fixed `compare_signature` macros.
    * Corrected `floor` and `ceiling` logic.
    * `*_compare` variants fixed to use correct macros.
* `macro_heap.h` / `macro_heap_sort.h`:

    * Fixed **replace** to check both children.
    * Safer push/pop code using `size_t`.
* `macro_introsort.h`:

    * Unified max-depth calculation with compiler builtins.
    * Smarter `push_and_loop` chooses smaller partition for recursion.
* `macro_to.h`:

    * Added strict overflow-safe parsing.
    * New **`macro_try_to_*` APIs** returning success flag.
    * Boolean conversion clarified (permissive vs strict).
* Added `macro_test.h`: header-only test harness (color output, assertions, timing).

### ✅ Tests

* New `tests/` with CMake integration & coverage target.
* Test harness build script (`tests/build.sh`).
* Added **comprehensive test suites**:

    * `test_macro_sort.c` — sorts, edge cases, thresholds, comparators.
    * `test_macro_bsearch.c` — value/KV, bounds, floor/ceiling, empty/singleton.
    * `test_macro_map.c` — unique/multimap, erase, iterators, copy, fuzz, rotate guards.
    * `test_macro_heap.c` — push/pop, replace edge cases, compare vs baked comparator.
    * `test_macro_cmp.c` — value and KV comparators, all styles.
    * `test_macro_time.c` — monotonicity, add\_seconds, diffs, composition.
    * `test_macro_to.c` — int/float/bool conversions, overflow handling, date formatting.
* Coverage runs `llvm-cov`/`gcov` across all test executables.

---

## Impact

* 🚀 Library is now header-only with clean install/export.
* 🛡️ Fixed correctness issues in bsearch floor/ceiling, heap replace, map rotation.
* 📖 Documentation is clearer, per-module, with quick reference and examples.
* ✅ High confidence with full test suite and coverage reports.
