# README.macro\_cmp.md — comparator “style” helpers for generated APIs

`macro_cmp.h` is a tiny header that standardizes how your generated functions (like those from `macro_map.h`) accept comparators. It lets you pick, at **code-generation time**, *what kind of comparator signature* your functions will expect:

* **cmp\_no\_arg**: `int cmp(const T *a, const T *b);`
* **cmp\_arg**:    `int cmp(const T *a, const T *b, void *arg);`
* **arg\_cmp**:    `int cmp(void *arg, const T *a, const T *b);`
* **less\_* variants*\* (return `bool` instead of `int`)
* **cmp/less “operator” style** (no function pointer; use built-in `<`, `==` on the pointed-to type)
* **KV comparators**: compare a *key* type `K` against a *value* type `V` (e.g., `int` key vs. `struct Node`)

This abstraction keeps your container macros simple: they just say “style = X”, and the header expands the right calls and function signatures.

---

## When should I use this?

* You’re generating a container API and want to choose **how** comparisons are made (with or without a context pointer).
* You want both **object-object** (`T*` vs `T*`) and **key-value** (`K*` vs `T*`) comparisons.
* You occasionally want to avoid function pointers entirely and rely on **native operators** (`<`, `==`) for PODs.

---

## The big picture

There are three families of macros:

1. **Value–Value** comparisons: `macro_less`, `macro_equal`, `macro_cmp`
2. **Key–Value** comparisons: `macro_*_kv(...)`
3. **Signature helpers**: `macro_cmp_signature(...)`, `macro_compare_signature(...)`, and their KV counterparts, which generate the *parameter list* of your generated functions.

Each family is **style-driven**. You pass a `style` token and the macros expand to the right form.

---

## Styles (what each one means)

### “cmp\_…” styles (return `int`)

* `cmp_no_arg` → `int cmp(const T*, const T*)`
* `cmp_arg`    → `int cmp(const T*, const T*, void *arg)`
* `arg_cmp`    → `int cmp(void *arg, const T*, const T*)`

### “less\_…” styles (return `bool`)

* `less_no_arg` → `bool less(const T*, const T*)`
* `less_arg`    → `bool less(const T*, const T*, void *arg)`
* `arg_less`    → `bool less(void *arg, const T*, const T*)`

### Operator styles (no function pointer)

* `less` and `cmp` → no callable; use `*(a) < *(b)` (and for equality, check both directions)

> In operator styles, `T` must be a scalar or a type where dereferencing and `<`/`==` make sense.

### KV variants

Same idea, but for `K` vs `V`:

* `cmp_no_arg`: `int cmp(const K*, const V*)`
* `cmp_arg`:    `int cmp(const K*, const V*, void *arg)`
* `arg_cmp`:    `int cmp(void *arg, const K*, const V*)`

> KV forms intentionally do **not** support `less_*` or operator styles: keys and values often differ, and we need a real function.

---

## Using the “value–value” helpers

### Compare two values (returns negative/zero/positive)

```c
int r = macro_cmp(cmp_no_arg, MyType, my_cmp, a_ptr, b_ptr);
// expands to: my_cmp((const MyType*)a_ptr, (const MyType*)b_ptr)
```

### Check ordering/equality

```c
bool lt = macro_less (cmp_no_arg, MyType, my_cmp, a_ptr, b_ptr); // my_cmp(a,b) < 0
bool eq = macro_equal(cmp_no_arg, MyType, my_cmp, a_ptr, b_ptr); // my_cmp(a,b) == 0
```

### Operator style (no function pointer)

```c
bool lt = macro_less (less, MyInt, /*unused*/, &x, &y);   // (*(x) < *(y))
bool eq = macro_equal(less, MyInt, /*unused*/, &x, &y);   // !(<) both ways
int  r  = macro_cmp  (cmp,  MyInt, /*unused*/, &x, &y);   // returns -1/0/1 via < tests
```

> The `cmp` operator style still uses `<` checks and returns -1/0/1; `less` operator style returns `bool`.

### With a context pointer

```c
void *ctx = ...;
int r = macro_cmp(cmp_arg, MyType, my_cmp_with_arg, a, b);          // cmp(a, b, arg)
int s = macro_cmp(arg_cmp, MyType, my_cmp_arg_first, a, b);         // cmp(arg, a, b)
```

---

## Using the “key–value” (KV) helpers

### Return an `int` result

```c
int r = macro_cmp_kv(cmp_no_arg, KeyType, ValType, kv_cmp, key_ptr, val_ptr);
// kv_cmp(const KeyType*, const ValType*)
```

### Convenience predicates

```c
bool eq = macro_equal_kv (cmp_no_arg, KeyType, ValType, kv_cmp, key_ptr, val_ptr);
bool lt = macro_less_kv  (cmp_no_arg, KeyType, ValType, kv_cmp, key_ptr, val_ptr);
bool le = macro_le_kv    (cmp_no_arg, KeyType, ValType, kv_cmp, key_ptr, val_ptr);
bool ge = macro_ge_kv    (cmp_no_arg, KeyType, ValType, kv_cmp, key_ptr, val_ptr);
bool gt = macro_greater_kv(cmp_no_arg, KeyType, ValType, kv_cmp, key_ptr, val_ptr);
```

### With a context pointer

```c
void *ctx = ...;
int r = macro_cmp_kv(cmp_arg, KeyType, ValType, kv_cmp_arg, key, val);   // cmp(key, val, arg)
int s = macro_cmp_kv(arg_cmp, KeyType, ValType, kv_arg_cmp, key, val);   // cmp(arg, key, val)
```

---

## Signature helpers (you’ll use these in generated APIs)

These build the **parameter list** of your function based on the style. Example with value–value compare:

```c
// Generate: Node *find(const macro_map_t *root,
//                      const Node *key,
//                      int (*cmp)(const Node*, const Node*))
value_type *find(const macro_map_t *root,
                 macro_cmp_signature_compare_cmp_no_arg(const Node *key, Node));
```

You normally won’t call the `_compare_*` helpers directly. Instead, use the **style-selector** form:

```c
// Using the style selector macro:
value_type *find(const macro_map_t *root,
                 macro_compare_signature(const Node *key, cmp_no_arg, Node));
```

The selector expands to the right signature:

| Style        | Expands to parameter list tail                       |
| ------------ | ---------------------------------------------------- |
| `cmp_no_arg` | `, int (*cmp)(const T*, const T*)`                   |
| `cmp_arg`    | `, int (*cmp)(const T*, const T*, void*), void *arg` |
| `arg_cmp`    | `, int (*cmp)(void*, const T*, const T*), void *arg` |
| `less_*`     | analogous forms but `bool (*cmp)(...)`               |
| `cmp`/`less` | *(no comparator parameter; operator style)*          |

For **KV**:

```c
value_type *find_kv(const macro_map_t *root,
    macro_compare_kv_signature(const Key *key, cmp_no_arg, Key, Value));
// → adds: , int (*cmp)(const Key*, const Value*)
```

---

## Practical examples

### 1) Plain “no-arg” comparators (recommended default)

```c
typedef struct Node { int k, v; } Node;

static int node_cmp_noarg(const Node *a, const Node *b) {
  return (a->k > b->k) - (a->k < b->k);
}
static int kv_cmp_noarg(const int *key, const Node *n) {
  return (*key > n->k) - (*key < n->k);
}
```

* In your generated functions, use `cmp_no_arg` style.
* Call sites pass `node_cmp_noarg` / `kv_cmp_noarg` automatically via generation, or as explicit parameters if you used `*_compare` variants.

### 2) Comparators with context (`void *arg`)

```c
typedef struct Ctx { int bias; } Ctx;
static int node_cmp_arg(const Node *a, const Node *b, void *arg) {
  Ctx *c = (Ctx*)arg;
  int ka = a->k + c->bias, kb = b->k + c->bias;
  return (ka > kb) - (ka < kb);
}
```

* Generate `*_compare` functions using `compare_cmp_arg` style for **value–value**, or `compare_cmp_arg` KV variants for key–value.
* At call time, pass both `cmp` and `arg`.

### 3) Operator style (no function pointer)

Good for POD scalars when you want *zero overhead*:

```c
int a = 5, b = 7;
bool lt = macro_less(less, int, /*unused*/, &a, &b);  // *(a) < *(b)
int  r  = macro_cmp (cmp,  int, /*unused*/, &a, &b);  // {-1,0,1} via <
```

> Not available for KV (because `K` and `V` differ).

---

## What the macros actually do (reference)

* **Value–value**

    * `macro_less(style, T, cmp, a, b)` → `< 0` via cmp; or `bool less(...)` via less\_\*; or `*(a) < *(b)` in operator style
    * `macro_equal(style, T, cmp, a, b)` → `== 0` via cmp; or derived from `less` in other styles
    * `macro_cmp(style, T, cmp, a, b)` → `int` comparator; for `less`/`cmp` operator style it’s synthesized from `<`
    * Shortcuts: `macro_le`, `macro_ge`, `macro_greater`

* **Key–value**

    * `macro_cmp_kv(style, K, V, cmp, a, b)` → `int` compare; styles: `cmp_no_arg`, `cmp_arg`, `arg_cmp`
    * Predicates: `macro_equal_kv`, `macro_less_kv`, `macro_le_kv`, `macro_ge_kv`, `macro_greater_kv`

* **Signatures**

    * `macro_cmp_signature(param, style, T)`
    * `macro_compare_signature(param, style, T)` → same as above but with a `compare_` prefix used by some generators
    * KV counterparts: `macro_cmp_kv_signature(...)`, `macro_compare_kv_signature(...)`

You typically **do not** call these directly in application code; they are used by container-generating macros (`macro_map_*`) to stamp out APIs with the right parameter lists and call patterns.

---

## Gotchas & tips

* **Return contract**: For `cmp_*` styles, your comparator must return `<0 / 0 / >0`. For `less_*`, return a strict weak ordering boolean.
* **Operator styles** assume `*(a) < *(b)` and (for equality) its inverse. Only use for types where this is correct.
* **KV** functions don’t support operator or `less_*` forms by design; use a real function.
* If you mix styles across your generated APIs, make sure the corresponding call sites pass the right parameters (`cmp` and optionally `arg`).
* The header is **header-only** and **pure macros**; no runtime cost beyond the comparator you choose.

---

## Minimal working snippet

```c
#include "the-macro-library/macro_cmp.h"

typedef struct Node { int k; } Node;

static int node_cmp_noarg(const Node *a, const Node *b) {
  return (a->k > b->k) - (a->k < b->k);
}
static int kv_cmp_noarg(const int *key, const Node *n) {
  return (*key > n->k) - (*key < n->k);
}

void demo(const Node *A, const Node *B, const int *k) {
  int r1 = macro_cmp(cmp_no_arg, Node, node_cmp_noarg, A, B);
  bool e1 = macro_equal(cmp_no_arg, Node, node_cmp_noarg, A, B);

  int r2 = macro_cmp_kv(cmp_no_arg, int, Node, kv_cmp_noarg, k, A);
  bool l2 = macro_less_kv(cmp_no_arg, int, Node, kv_cmp_noarg, k, A);

  (void)r1; (void)e1; (void)r2; (void)l2;
}
```

---

## License

SPDX-License-Identifier: Apache-2.0
Copyright © 2019–2025 Andy Curtis
