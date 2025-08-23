# README.macro\_sort.md — header-only, type-safe introsort (with fast “already sorted / reversed” checks)

`macro_sort.h` generates specialized, inlined **introsort** functions for your element type and comparison style (shared with the rest of the macro library via `macro_cmp.h`). It’s header-only, allocation-free, and aims to be *way* faster than `qsort` in real code by:

* inlining your comparator (no function pointer calls in the hot loop),
* using **introsort** (quicksort that switches to heapsort to guarantee O(n log n)),
* and doing an **early array shape check** up front to fast-path already-sorted or fully reversed inputs.

If you know C++’s `std::sort`, think of this as a zero-overhead C version that’s easy to drop into C projects.

---

## What you include

```c
#include "the-macro-library/macro_sort.h"
```

Internally, this pulls in `macro_introsort.h`, but you never need to include that directly.

---

## Quick start (baked-in comparator)

```c
#include <stdio.h>
#include <stdbool.h>
#include "the-macro-library/macro_sort.h"

/* A strict-weak-order “less” for ints */
static inline bool int_less(const int *a, const int *b) { return *a < *b; }

/* Generate a type-safe sort: void sort_ints(int *base, size_t n) */
macro_sort(sort_ints, int, int_less);

int main(void) {
    int a[] = {5,4,3,1,2};
    sort_ints(a, sizeof a/sizeof a[0]);

    for (size_t i=0;i<sizeof a/sizeof a[0];++i) printf(" %d", a[i]);
    printf("\n");
    return 0;
}
```

That’s it. You now have a fully inlined, type-checked sort specialized for `int`.

---

## Why it’s fast

* **Introsort**: starts with quicksort, monitors recursion depth, and falls back to heapsort to avoid quadratic cases.
* **Early shape check**: before partitioning, it compares *first / mid / last* (and a few probes) to detect:

    * already sorted → linear verification pass → return,
    * fully reversed → linear reverse → return,
    * otherwise → proceed with introsort.
* **Inlining**: the comparator and the loop live in your TU, so the compiler can optimize aggressively (no indirect calls).

Numbers in the top-level README show large wins vs `qsort` and strong results vs `std::sort` (especially for already-sorted / reversed arrays and when C++ must call a runtime comparator).

---

## The macros you’ll use

There are two families:

1. **Bake a comparator at generation time** (fastest call-site)

```c
#define macro_sort(name, type, cmp_less_fn)
```

* Generates: `void name(type *base, size_t n);`
* Uses default comparison style `less_no_arg` (i.e., `bool less(const type*, const type*)`).
* For other compare *styles* (e.g., `cmp_no_arg` with `int cmp(const T*, const T*)`), use the underscore form:

  ```c
  _macro_sort(name, style, type, cmp_fn)
  ```

  where `style` is from `macro_cmp.h` (see **Comparator styles** below).

2. **Comparator at call-time** (pass a function pointer each call)

```c
#define macro_sort_compare(name, type)
```

* Generates: `void name(type *base, size_t n, bool (*cmp)(const type*, const type*));`
* Uses the default style wrapper internally; there’s also:

  ```c
  _macro_sort_compare(name, style, type)
  ```

  if you want other compare signatures (e.g., `cmp_no_arg`, `cmp_arg`, …).

### Header-only declarations

For headers, use the `_h` variants to generate **prototypes**:

```c
/* in a header (don’t forget the semicolon) */
macro_sort_h(sort_ints, int);            // prototype for baked comparator version
macro_sort_compare_h(sort_ints, int);    // prototype for call-time comparator version
```

You can prefix with `static` / `static inline` in the line above if desired:

```c
static inline
macro_sort_h(sort_ints, int);
```

---

## Comparator styles (from `macro_cmp.h`)

All algorithms in this library share a unified set of comparison “styles”. For sorting you’ll typically use one of:

| Style                   | Comparator signature                 | Notes                                 |
| ----------------------- | ------------------------------------ | ------------------------------------- |
| `less_no_arg` (default) | `bool less(const T*, const T*)`      | Returns `true` iff `*a < *b`          |
| `cmp_no_arg`            | `int  cmp (const T*, const T*)`      | Negative/zero/positive tri-cmp        |
| `less_arg` / `cmp_arg`  | `...(const T*, const T*, void *arg)` | Call-site passes an extra `void* arg` |
| `arg_less` / `arg_cmp`  | `...(void *arg, const T*, const T*)` | `arg` in first position               |

Pick the style that matches your comparator function.

* If you **bake** the comparator: `_macro_sort(name, style, type, cmp_fn)`.
* If you **pass** the comparator each call: `_macro_sort_compare(name, style, type)`.

> Tip: prefer `less_*` for readability or `cmp_*` if you already have tri-cmp functions.

---

## More examples

### 1) Sorting a struct array by a field (baked comparator)

```c
typedef struct { int key; double val; } Pair;

static inline bool pair_less(const Pair *a, const Pair *b) {
    return a->key < b->key || (a->key == b->key && a->val < b->val);
}

/* Generates: void sort_pairs(Pair *base, size_t n) */
macro_sort(sort_pairs, Pair, pair_less);
```

### 2) Call-time comparator (same binary, different orders)

```c
/* One generator, many comparisons */
macro_sort_compare(sort_pairs_cmp, Pair);

static inline bool by_key_asc (const Pair *a, const Pair *b){ return a->key < b->key; }
static inline bool by_key_desc(const Pair *a, const Pair *b){ return a->key > b->key; }

void use(Pair *p, size_t n) {
    sort_pairs_cmp(p, n, by_key_asc);
    sort_pairs_cmp(p, n, by_key_desc);
}
```

### 3) Using a tri-cmp (`cmp_no_arg`)

```c
static inline int cmp_pair(const Pair *a, const Pair *b) {
    if (a->key != b->key) return (a->key < b->key) ? -1 : 1;
    return (a->val < b->val) ? -1 : (a->val > b->val);
}

/* Tri-cmp style: cmp_no_arg */
_macro_sort(sort_pairs_tricmp, cmp_no_arg, Pair, cmp_pair);
```

### 4) With an extra argument (`cmp_arg`)

```c
typedef struct { int primary; int secondary; } KeyOrder;

static int cmp_pair_arg(const Pair *a, const Pair *b, void *arg) {
    const KeyOrder *ord = (const KeyOrder*)arg;
    int da = (ord->primary   ? a->key : (int)a->val);
    int db = (ord->primary   ? b->key : (int)b->val);
    if (da != db) return (da < db) ? -1 : 1;
    da = (ord->secondary ? a->key : (int)a->val);
    db = (ord->secondary ? b->key : (int)b->val);
    return (da < db) ? -1 : (da > db);
}

/* Generate prototype that accepts comparator + arg */
_macro_sort_compare(sort_pairs_cfg, cmp_arg, Pair);

/* call */
KeyOrder ord = { .primary = 1, .secondary = 0 };
sort_pairs_cfg(arr, n, cmp_pair_arg, &ord);
```

---

## Static / inline

You can make the generated function `static` or `static inline` simply by putting the storage specifier on the line before the macro:

```c
static inline
macro_sort(sort_pairs, Pair, pair_less);
```

Same for the `_h` prototypes in public headers.

---

## API cheat sheet

```c
/* Default style is less_no_arg (bool less(const T*, const T*)) */
macro_sort_h(name, T)                    // prototype
macro_sort  (name, T, less_fn)           // definition

/* Accept comparator at call-time (default style) */
macro_sort_compare_h(name, T)            // prototype
macro_sort_compare  (name, T)            // definition

/* Full control over style/signature (underscore forms) */
_macro_sort_h        (name, style, T)              // prototype
_macro_sort          (name, style, T, cmp_fn)      // definition
_macro_sort_compare_h(name, style, T)              // prototype
_macro_sort_compare  (name, style, T)              // definition

/* style is one of: less_no_arg (default), less_arg, arg_less, cmp_no_arg, cmp_arg, arg_cmp, ... */
```

---

## Correctness expectations

Your comparator must define a **strict weak ordering**:

* Irreflexive: not `(a < a)`
* Transitive: if `(a < b)` and `(b < c)` then `(a < c)`
* Transitive incomparability (“equivalent” items behave consistently)

Violating this can cause non-termination or undefined ordering.

---

## Practical tips

* **Performance**: The baked-in (`macro_sort`) variant is generally fastest (the compiler can inline the comparator and remove branches). The `*_compare` variant is flexible when you need to pick comparators at runtime.
* **Small arrays**: The implementation uses insertion sort for tiny partitions for cache friendliness.
* **Stability**: This sort is **not stable** (like `std::sort`). If you need stability, do a Schwartzian transform (decorate → sort → undecorate) or add a tie-breaker in your comparator.
* **Debugging**: Use the project’s `bin/convert-macros-to-code` to expand macros into concrete C you can step through in a debugger.

---

## Interop with other library parts

* Comparators are shared across **sort**, **bsearch**, and **map**. If you already wrote a tri-cmp (or less) for one algorithm, you can reuse it for the others without glue code.
* If you rely on `macro_bsearch_*`, you’ll typically pass the same comparator you used for sorting to guarantee consistent results.

---

## License

SPDX-License-Identifier: Apache-2.0
© 2019–2025 Andy Curtis

---

If you want, I can also add a tiny `examples/demo/sort_pairs.c` that builds under your tree and shows both baked and call-time comparators.
