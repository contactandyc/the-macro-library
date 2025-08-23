# README.macro\_bsearch.md — header-only, zero-overhead binary search generators

`macro_bsearch.h` lets you **generate** specialized, inlined binary-search functions for your types and comparator style (the same styles used by `macro_cmp.h`). You get fast, branch-lean, allocation-free lookups on **sorted arrays** with optional **duplicate-handling** variants (first/last/lower/upper/floor/ceiling). Everything is header-only.

---

## What gets generated?

You choose a **name**, your **type(s)**, and a **comparator style**. The macro expands into a real C function with this shape:

* Value–value search (key and array elements have the same type `T`)

  ```c
  T *name(const T *key, const T *base,
          /* ... comparator params depend on style ... */,
          size_t n);
  ```

* Key–value (KV) search (key type `K`, array element type `V`)

  ```c
  V *name(const K *key, const V *base,
          /* ... comparator params depend on style ... */,
          size_t n);
  ```

All variants return a **pointer into `base`** (when found) or `NULL` (when not found / out of range). The array must be sorted in **nondecreasing** order under your comparator.

---

## Pick your flavor

Every flavor exists in both value–value and KV forms.

| Flavor        | Meaning on arrays with duplicates                                     |
| ------------- | --------------------------------------------------------------------- |
| `core`        | A classic `bsearch`: returns **any** matching element (not specified) |
| `first`       | Returns pointer to the **first** element equal to `key`               |
| `last`        | Returns pointer to the **last** element equal to `key`                |
| `lower_bound` | First element **≥ key** (a.k.a. `lb`)                                 |
| `upper_bound` | First element **> key** (a.k.a. `ub`)                                 |
| `floor`       | Last element **≤ key**                                                |
| `ceiling`     | First element **≥ key** (same position as `lower_bound`)              |

> If “no such element” exists, these return `NULL`.

Time complexity: **O(log n)**.
All work is done with indices; no allocations.

---

## Comparator “styles” (powered by `macro_cmp.h`)

Exactly like the map API, you pick a style:

* `cmp_no_arg`: `int cmp(const T*, const T*)`
* `cmp_arg`:    `int cmp(const T*, const T*, void *arg)`
* `arg_cmp`:    `int cmp(void *arg, const T*, const T*)`

KV uses analogous signatures with `K` vs `V`. The default style is `cmp_no_arg`.

There are two families of generator macros:

* **“…\_compare” variants**: the generated function *takes* a comparator pointer (and optional `arg`) as parameters.
* **Non-compare variants**: you bake in a named comparator at generation time.

---

## Quick reference — value–value generators

> Replace `name` and `T` with your own.

**With baked-in comparator** (you pass a comparator *name* at generation time; call site passes just `key, base, n`):

```c
macro_bsearch_h               (name, T);                 // declares: T *name(const T*, const T*, size_t n)
macro_bsearch                 (name, T, cmp);            // core
macro_bsearch_first           (name, T, cmp);
macro_bsearch_last            (name, T, cmp);
macro_bsearch_lower_bound     (name, T, cmp);
macro_bsearch_upper_bound     (name, T, cmp);
macro_bsearch_floor           (name, T, cmp);
macro_bsearch_ceiling         (name, T, cmp);
```

**Comparator passed at call site** (style = `macro_bsearch_default()` = `cmp_no_arg` unless you’ve redefined it):

```c
macro_bsearch_compare_h           (name, T);             // declares with comparator parameters
macro_bsearch_compare             (name, T);             // core
macro_bsearch_first_compare       (name, T);
macro_bsearch_last_compare        (name, T);
macro_bsearch_lower_bound_compare (name, T);
macro_bsearch_upper_bound_compare (name, T);
macro_bsearch_floor_compare       (name, T);
macro_bsearch_ceiling_compare     (name, T);
```

The “…\_compare” versions’ signatures expand using `macro_compare_signature(...)` from `macro_cmp.h`. For `cmp_no_arg`, it adds:

```c
, int (*cmp)(const T*, const T*)
```

For `cmp_arg`, it adds:

```c
, int (*cmp)(const T*, const T*, void*), void *arg
```

(and similarly for `arg_cmp`).

---

## Quick reference — KV generators

> Key and value types can differ (`K` vs `V`).

**Baked-in comparator:**

```c
macro_bsearch_kv_h               (name, K, V);
macro_bsearch_kv                 (name, K, V, cmp);          // core
macro_bsearch_first_kv           (name, K, V, cmp);
macro_bsearch_last_kv            (name, K, V, cmp);
macro_bsearch_lower_bound_kv     (name, K, V, cmp);
macro_bsearch_upper_bound_kv     (name, K, V, cmp);
macro_bsearch_floor_kv           (name, K, V, cmp);
macro_bsearch_ceiling_kv         (name, K, V, cmp);
```

**Comparator passed at call site:**

```c
macro_bsearch_kv_compare_h           (name, K, V);
macro_bsearch_kv_compare             (name, K, V);      // core
macro_bsearch_first_kv_compare       (name, K, V);
macro_bsearch_last_kv_compare        (name, K, V);
macro_bsearch_lower_bound_kv_compare (name, K, V);
macro_bsearch_upper_bound_kv_compare (name, K, V);
macro_bsearch_floor_kv_compare       (name, K, V);
macro_bsearch_ceiling_kv_compare     (name, K, V);
```

The signature tail comes from `macro_compare_kv_signature(...)`.

---

## Minimal examples

### 1) Value–value, baked-in comparator (no comparator params at call site)

```c
#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_bsearch.h"

typedef struct { int key, payload; } Item;

static int item_cmp(const Item *a, const Item *b) {
  return (a->key > b->key) - (a->key < b->key);
}

macro_bsearch_h          (item_find, Item);
macro_bsearch            (item_find, Item, item_cmp);
macro_bsearch_lower_bound(item_lb,   Item, item_cmp);

void demo(void) {
  Item arr[] = { {1,10}, {3,30}, {3,31}, {8,80} }; // sorted by key!
  size_t n = sizeof arr / sizeof arr[0];

  Item key = {3,0};
  Item *any = item_find(&key, arr, n);     // → one of the {3,30}/{3,31}
  Item *lb  = item_lb(&key, arr, n);       // → first {3,30}
  (void)any; (void)lb;
}
```

### 2) KV, comparator passed at call site (no baking)

```c
#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_bsearch.h"

typedef struct { int key, payload; } Item;

static int kv_cmp(const int *k, const Item *v) {
  return (*k > v->key) - (*k < v->key);
}

macro_bsearch_kv_compare_h(item_first_kv, int, Item);
macro_bsearch_first_kv_compare(item_first_kv, int, Item);

void demo2(void) {
  Item arr[] = { {1,10}, {3,30}, {3,31}, {8,80} };
  size_t n = sizeof arr / sizeof arr[0];
  int k = 3;

  Item *first = item_first_kv(&k, arr, kv_cmp, n);
  // If you used cmp_arg/arg_cmp styles, pass (cmp, arg) accordingly.
  (void)first;
}
```

---

## Contract & correctness checklist

* **Sorted input required**: `base[0..n)` must be sorted in **nondecreasing** order under your comparator.
  If you use `*_kv`, the array must be sorted by **the key projection** used by your KV comparator.

* **Stability**: These searches do **not** reorder data; they only read.

* **Duplicates**:

    * Use `first`/`last` to locate the ends of an equal range.
    * `lower_bound` returns the left edge of the range `== key`.
    * `upper_bound` returns the first element **strictly greater** than `key`.

* **Floor/Ceiling**:

    * `floor`: last element `≤ key` (or `NULL` if all elements > key)
    * `ceiling`: first element `≥ key` (or `NULL` if all elements < key)

* **Const-correctness**: The API returns a `T*`/`V*` into `base`. If your array is `const`, cast the return to `const T*` as needed (or wrap with a const-friendly alias).

* **Comparator discipline**:

    * For `cmp_*` styles, your comparator must obey the usual three-way semantics and a strict weak ordering.
    * For KV: the same, but comparing a `K*` to a `V*` (typically by projecting `V`’s key field).
    * If you use a context pointer (`cmp_arg` / `arg_cmp`), ensure the same `arg` is used throughout.

* **Bounds**: Passing `n == 0` is valid; all variants return `NULL`.

---

## Choosing between baked-in vs. compare versions

* **Baked-in** (`macro_bsearch_*`): zero comparator overhead at call sites, great for hot paths with a single comparator. The comparator symbol must be visible where you instantiate the macro.

* **Compare versions** (`macro_bsearch_*_compare`): more flexible; you pass the comparator (and optional arg) per call. Perfect for libraries that must expose a single function that works with different comparators.

You can mix both in the same TU if you need multiple named searchers.

---

## Tips & patterns

* **Equal range** (start, end) using `lb`/`ub`:

  ```c
  V *lb = lower_bound(...);
  V *ub = upper_bound(...);
  // elements in [lb, ub) are equal to key
  size_t count = lb && ub ? (size_t)(ub - lb) : 0;  // valid because the pointer arithmetic is within the same base array
  ```

* **Existence check**:

  ```c
  V *lb = lower_bound(...);
  bool exists = lb && (kv_cmp(key, lb) == 0); // or value–value cmp for non-KV
  ```

* **Index math**: If you need indices, compute `size_t idx = (size_t)(ptr - base);` after a non-NULL result.

---

## FAQ

**Q: Do I need to include `macro_cmp.h`?**
A: Yes. `macro_bsearch.h` relies on its signature/comparator helpers. Include both.

**Q: What about descending order arrays?**
A: Not supported directly. Either sort ascending or wrap your comparator to invert the sense.

**Q: Thread safety?**
A: Pure functions over your input range; they are reentrant and thread-safe so long as your comparator is.

**Q: Can I use non-trivial key projections in KV?**
A: Yes—your KV comparator can look inside complex `V` to compare with `K`. Just keep it a strict weak ordering consistent with your array’s sort order.

---

## License

SPDX-License-Identifier: Apache-2.0
Copyright © 2019–2025 Andy Curtis
