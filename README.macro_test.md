# README.macro\_test.md — tiny zero-dep C test harness

`macro_test.h` is a **header-only** testing harness for C (and C++ via `extern "C"`). It’s ideal for projects that want:

* dead-simple test declarations (`MACRO_TEST`, `MACRO_ADD`, `macro_run_all`),
* colorful PASS/FAIL lines,
* optional per-test timing in microseconds,
* no external dependencies, no linker flags, no build system magic.

It’s intentionally minimal: one header, a small set of assertions, and a runner.

---

## Quick start

```c
#include "the-macro-library/macro_test.h"

MACRO_TEST(example_addition) {
    int x = 2 + 2;
    MACRO_ASSERT_EQ_INT(x, 4);
}

int main(void) {
    macro_test_case tests[16];
    size_t test_count = 0;

    MACRO_ADD(tests, example_addition);

    macro_run_all("my_suite", tests, test_count);
    return 0;
}
```

Output (colors enabled by default):

```
[====] Running 1 tests in my_suite
[PASS] example_addition
[====] 1/1 passed
```

---

## Declaring tests

* `MACRO_TEST(name)`
  Expands to `static void name(void)`. Put your assertions inside.

* `MACRO_ADD(array, test_fn)`
  Appends `{ "test_fn", test_fn }` to your test array and increments `test_count`.

Typical pattern:

```c
MACRO_TEST(test_push_pop)   { /* ... */ }
MACRO_TEST(test_negative)   { /* ... */ }
MACRO_TEST(test_edge_cases) { /* ... */ }

int main(void) {
    macro_test_case tests[64];
    size_t test_count = 0;

    MACRO_ADD(tests, test_push_pop);
    MACRO_ADD(tests, test_negative);
    MACRO_ADD(tests, test_edge_cases);

    macro_run_all("stack", tests, test_count);
}
```

---

## Running a suite

* `macro_run_all(const char* suite, macro_test_case *t, size_t n)`

Prints a header once, then runs each test in order. On success it prints `[PASS]`, on failure the assertion prints an immediate `[FAIL]` line and calls `assert()`, which **aborts** the process (so remaining tests won’t run).

If you want “keep running after failures,” see **Continuing after failures** below.

---

## Assertions

All assertions print a **one-line FAIL** header with file\:line, then call `assert(...)`.
They also print helpful details (actual vs expected) when available.

* `MACRO_ASSERT_TRUE(expr)`
  Fails if `!(expr)`.

* `MACRO_ASSERT_FALSE(expr)`
  Fails if `(expr)`.

* `MACRO_ASSERT_EQ_INT(a, b)`
  Compares as `int`. Shows both values on failure.

* `MACRO_ASSERT_EQ_SZ(a, b)`
  Compares as `size_t`. Shows both values on failure.

* `MACRO_ASSERT_STREQ(a, b)`
  Null-safe string equality (`NULL` equals `NULL`). Prints both strings.

Utility:

* `macro_streq(const char *a, const char *b)`
  Returns `1` if equal (including both `NULL`), else `0`.

> Need more flavors? It’s trivial to add `MACRO_ASSERT_EQ_DBL`, `MACRO_ASSERT_NEQ_INT`, etc. Keep the fail format consistent for greppability in CI logs.

---

## Output customization

### Colors

* Controlled by `MACRO_TEST_ENABLE_COLOR` (default `1`).
* Disable at compile time:

  ```bash
  cc ... -DMACRO_TEST_ENABLE_COLOR=0
  ```

### Per-test timings

* Controlled by `MACRO_TEST_ENABLE_TIMING_US` (default `0`).
* Enable at compile time:

  ```bash
  cc ... -DMACRO_TEST_ENABLE_TIMING_US=1
  ```

  Prints:

  ```
  [PASS] test_name (+123 us)
  ```

  Uses `gettimeofday` (portable enough for Linux/macOS). On Windows you’ll need a POSIX layer or keep it off.

---

## Common patterns

### 1) Deterministic fuzz

```
[seed] unique=12345, mmap=4242, fuzz1=777, fuzz2=4242
```

If your tests use RNG, print seeds at the top of `main()` so a failure can be reproduced. Example:

```c
#include <time.h>

int main(void) {
    unsigned unique = 12345, fuzz1 = 777;
    printf("[seed] unique=%u, fuzz1=%u\n", unique, fuzz1);
    srand(unique);

    /* register and run tests ... */
}
```

### 2) Fixtures (setup/teardown)

No special API—just write helpers:

```c
static void setup_array(int *a, size_t n) { /* ... */ }
static void teardown_array(int *a)         { /* ... */ }

MACRO_TEST(sorts_ascending) {
    int a[1024];
    setup_array(a, 1024);
    /* run function under test */
    teardown_array(a);
}
```

### 3) Continuing after failures

By design, a failed assertion aborts the process (uses `assert`). If you prefer “collect all failures,” duplicate the macros locally and remove the `assert(...)` calls, or gate them behind a project define:

```c
#ifdef MACRO_TEST_SOFT_FAIL
  #define MACRO__BREAK() ((void)0)
#else
  #define MACRO__BREAK() assert(0)
#endif
```

Then use `MACRO__BREAK()` in the fail path.

### 4) Filtering tests

This harness doesn’t implement filtering. If you need it quickly:

* Use your build system to compile only selected test files, **or**
* Add a simple filter in `main()`:

```c
const char *only = getenv("ONLY");
for (size_t i=0;i<test_count;i++) {
    if (only && !strstr(tests[i].name, only)) continue;
    run_one(tests[i]);
}
```

---

## C vs C++ integration

* Header wraps declarations with `extern "C"` when compiled as C++.
* You can write tests in C or C++; just include the header and call C functions as usual.

---

## CI tips

* **Exit status**: A failing test triggers `assert`, which exits non-zero. CI will mark the job as failed.
* **No flaky tests**: Because a failed assert aborts the process, one flaky test hides later tests. Either de-flake or adopt the optional “soft fail” approach described above during triage.
* **Readable logs**: Output is single line per pass and concise on failures; grep for `\[FAIL]` to jump around.

---

## Portability notes

* Linux/macOS: everything works out of the box.
* Windows: color escape codes show as raw text in vanilla `cmd.exe` (use Windows Terminal/WT or disable colors). Timing uses `gettimeofday` if enabled—turn timing off unless you provide a replacement.

---

## API reference

```c
/* Types */
typedef void (*macro_test_fn)(void);
typedef struct { const char *name; macro_test_fn fn; } macro_test_case;

/* Decl helpers */
static inline int  macro_streq(const char*, const char*);
static inline void macro_run_all(const char *suite, macro_test_case *t, size_t n);

/* Define a test */
#define MACRO_TEST(name)          static void name(void)

/* Register a test (in order) */
#define MACRO_ADD(list, fn)       do { list[test_count++] = (macro_test_case){#fn, fn}; } while (0)

/* Assertions */
#define MACRO_ASSERT_TRUE(expr)
#define MACRO_ASSERT_FALSE(expr)
#define MACRO_ASSERT_EQ_INT(a,b)
#define MACRO_ASSERT_EQ_SZ(a,b)
#define MACRO_ASSERT_STREQ(a,b)
```

---

## Example: full file

```c
#include <stdio.h>
#include "the-macro-library/macro_test.h"

/* Unit under test */
static int add(int a, int b) { return a + b; }

/* Tests */
MACRO_TEST(add_simple) {
    MACRO_ASSERT_EQ_INT(add(2,2), 4);
    MACRO_ASSERT_TRUE(add(1,0) == 1);
}

MACRO_TEST(add_zero) {
    MACRO_ASSERT_EQ_INT(add(0,5), 5);
}

int main(void) {
    macro_test_case tests[16];
    size_t test_count = 0;

    MACRO_ADD(tests, add_simple);
    MACRO_ADD(tests, add_zero);

    macro_run_all("math/add", tests, test_count);
    return 0;
}
```

Compile & run:

```bash
cc -O2 -Wall -Wextra test_add.c -o test_add
./test_add
```

---

## License

SPDX-License-Identifier: Apache-2.0
© 2019–2025 Andy Curtis, © 2024–2025 Knode.ai
