# README.macro\_map.md — intrusive ordered map (red–black tree)

`macro_map` is a set of header-only macros that generate **type-safe, zero-allocation, intrusive** ordered maps and multimaps.
It’s built around a tiny link type:

```c
typedef struct macro_map_s {
  uintptr_t parent_color;   // parent pointer + color in LSB
  struct macro_map_s *left;
  struct macro_map_s *right;
} macro_map_t;
```

You embed a `macro_map_t` **inside your own record type** and the macros generate `insert`, `find`, `bounds`, `iterate`, `erase`, and `copy` functions that operate on your type.

---

## Quick start

### 1) Define your node type

If you can put the link **first** (fast path):

```c
typedef struct Node {
  macro_map_t link;   // first member (fast-path API)
  int key;
  int value;
} Node;
```

If you cannot put it first, you can still use the `_with_field` variants (see below).

### 2) Define comparators

“no-arg” comparators (the default) are just functions:

```c
// compare two Node* by key
static int node_cmp_noarg(const Node *a, const Node *b) {
  return (a->key > b->key) - (a->key < b->key);
}

// compare an int key with a Node*
static int kv_cmp_noarg(const int *k, const Node *n) {
  return (*k > n->key) - (*k < n->key);
}
```

> If you need to pass a comparator (or an extra context) at call time, use the `*_compare` macros — see “Comparator styles” below.

### 3) Generate functions

Pick **first-member** or **with-field** flavor:

```c
/* First-member (link first in struct) */
macro_map_insert        (map_insert,        Node, node_cmp_noarg);
macro_map_find_kv       (map_find_by_key,   int, Node, kv_cmp_noarg);

/* With-field (link is not first) */
macro_map_insert_with_field (map_insert_wf,  link, Node, node_cmp_noarg);
macro_map_find_kv_with_field(map_find_wf,   link, int, Node, kv_cmp_noarg);

/* Multimap (allow duplicates) */
macro_multimap_insert_with_field(mmap_insert, link, Node, node_cmp_noarg);

/* Bounds helpers (kv) */
_macro_map_kv_with_field(map_lower_bound, link, lower_bound, macro_map_default(),
                         int, Node, kv_cmp_noarg);
_macro_map_kv_with_field(map_upper_bound, link, upper_bound, macro_map_default(),
                         int, Node, kv_cmp_noarg);
```

### 4) Use it

```c
macro_map_t *root = NULL;

Node n1 = {.key = 10, .value = 1};
Node n2 = {.key = 5,  .value = 2};
Node n3 = {.key = 15, .value = 3};

map_insert(&root, &n1);
map_insert(&root, &n2);
map_insert(&root, &n3);

/* find by key (kv comparator) */
int k = 10;
Node *hit = map_find_by_key(root, &k);

/* iterate (in-order) */
for (macro_map_t *p = macro_map_first(root); p; p = macro_map_next(p)) {
  Node *node = (Node *)p;                // fast path when link is first member
  // if not first member: Node *node = macro_parent_object(p, Node, link);
}

/* erase a node you have a pointer to */
macro_map_erase(&root, &n2.link);

/* copy a whole tree (deep copy of payload via callback) */
static macro_map_t *copy_cb(macro_map_t *lnk, void *arg) {
  (void)arg;
  Node *src = (Node *)lnk;               // or macro_parent_object(lnk, Node, link)
  Node *dst = malloc(sizeof(*dst));
  *dst = *src;
  return &dst->link;
}
macro_map_t *copy = macro_map_copy(root, copy_cb, NULL);

/* free a tree in post-order */
for (macro_map_t *p = macro_map_postorder_first(copy); p; ) {
  macro_map_t *next = macro_map_postorder_next(p);
  free((Node *)p);
  p = next;
}
```

---

## API surface (what gets generated)

All public functions are created by macros, so you decide the names.

### Inserts

* **Unique keys (map)**

    * `macro_map_insert(NAME, TYPE, cmpfn)` (first-member)
    * `macro_map_insert_with_field(NAME, FIELD, TYPE, cmpfn)` (arbitrary field)
* **Allow duplicates (multimap)**

    * `macro_multimap_insert(NAME, TYPE, cmpfn)` / `_with_field(...)`

> Duplicate ordering in multimap is stable and deterministic: ties are ordered by **node address**.

### Lookup (exact match)

* `macro_map_find(NAME, TYPE, cmpfn)` / `_with_field(...)` – takes a full `TYPE*` key
* `macro_map_find_kv(NAME, K, V, cmpfn)` / `_with_field(...)` – takes just a key of type `K`

### Bounds & neighbors (KV comparators)

* `first`, `last`, `lower_bound`, `upper_bound`, `floor`, `ceiling`

    * via internal helpers:
      `_macro_map_kv_with_field(NAME, FIELD, lower_bound, macro_map_default(), K, V, cmpfn)`
      (same for `first`, `last`, `upper_bound`, `floor`, `ceiling`)

### Iteration

* `macro_map_first(root)`, `macro_map_last(root)`
* `macro_map_next(n)`, `macro_map_previous(n)`
* `macro_map_postorder_first(root)`, `macro_map_postorder_next(n)` (useful for destroy/free)

### Erase / Copy

* `macro_map_erase(&root, node_link_ptr)` – removes the given node from the tree
* `macro_map_copy(root, copy_cb, arg)` – deep copy using your callback

---

## Comparator styles

There are two families:

1. **No-arg (default)** — You pass a function pointer whose signature is “just compare two values” (or key vs value). Use:

    * `macro_map_insert`, `macro_map_find_kv`, etc.
    * Shorthand: `macro_map_default()` expands to `cmp_no_arg`.

2. **Compare-at-call (advanced)** — You generate functions that accept a comparator at call time (useful when the comparator or its context isn’t known at compile time). Look for the `*_compare` variants, e.g.:

    * `macro_map_insert_compare(NAME, TYPE)` → function takes comparator parameter
    * `macro_map_find_kv_compare(NAME, K, V)` → function takes kv comparator

(These rely on signatures from `macro_cmp.h` and are drop-in replacements with an extra comparator parameter.)

---

## Two layout flavors: “first member” vs “with field”

* **First-member flavor** is the simplest and fastest: you can treat `macro_map_t*`
  as your node pointer and avoid `macro_parent_object` casts.
* **With-field flavor** works when `macro_map_t` is not the first member; you tell the generator which field to use: `..._with_field(NAME, FIELD, ...)`. When iterating, use:

  ```c
  Node *node = macro_parent_object(link_ptr, Node, link);
  ```

---

## Big-O & behavior

* All operations are **O(log n)** (red–black tree).
* **No allocations** are performed by the map itself; you manage memory for nodes.
* **Unique map**: a second insert of an equivalent key returns `false`.
* **Multimap**: duplicates are allowed; ties are ordered by node address.
* `erase` requires the **exact node** pointer (intrusive).
* **Thread safety**: not thread-safe; add your own synchronization if needed.

---

## Copying trees

`macro_map_copy(root, copy_cb, arg)` does a structural clone:

* It walks the source tree, calls your `copy_cb` for each node link.
* You allocate a new payload, return its `macro_map_t*` link.
* Link fields (parent/left/right/color) are set up by the copier.

This pattern is perfect for building a new pool/arena without disturbing the original tree.

---

## Freeing trees

Use the **post-order traversal** helpers to free nodes without recursion:

```c
for (macro_map_t *p = macro_map_postorder_first(root); p; ) {
  macro_map_t *next = macro_map_postorder_next(p);
  free((Node *)p);                                 // or macro_parent_object(p, Node, link)
  p = next;
}
```

---

## Common pitfalls & tips

* **Initialize your node fields** however you want; the library will set the link on insert:
  `_macro_map_fix_insert` zeroes `left/right`, sets parent/color, and rebalances.
* **Do not insert the same node into multiple maps at once.**
* **Do not stack-allocate nodes** that outlive the map usage (unless you’re sure about lifetime).
* If you use the **with-field** flavor, remember to cast iterator links back to your type using `macro_parent_object`.
* On **multimap**, when you walk a range of equal keys:

    * `lb = lower_bound(k)`, `ub = upper_bound(k)` delimit the half-open interval `[lb, ub)`.
    * Iterate via `for (macro_map_t *p = &lb->link; p != (ub? &ub->link : NULL); p = macro_map_next(p))`.

---

## Portability & alignment notes

* The link stores the parent pointer **and the color bit** in `parent_color` using the **LSB**.
* `macro_map_t` is enforced to be **at least 2-byte aligned** (via `__attribute__((aligned(2)))` / `__declspec(align(2))` and a static assert on C11 compilers). This guarantees the LSB of any valid parent pointer is 0, so it can hold the color bit safely.
* The code uses `uintptr_t` for the packed pointer; the header asserts `sizeof(uintptr_t) >= sizeof(void*)`. On platforms lacking `<stdint.h>`, it falls back to `size_t` as a best effort.

---

## Example: multimap range

```c
/* build multimap with duplicates */
macro_map_t *root = NULL;
Node a={.key=2}, b={.key=2}, c={.key=2}, d={.key=4};
mmap_insert(&root, &a);
mmap_insert(&root, &b);
mmap_insert(&root, &c);
mmap_insert(&root, &d);

/* walk all 2's using lower/upper bound */
int k = 2;
Node *lb = map_lower_bound(root, &k);
Node *ub = map_upper_bound(root, &k);
for (macro_map_t *p = lb ? &lb->link : NULL; p && p != (ub ? &ub->link : NULL); p = macro_map_next(p)) {
  Node *n = (Node *)p;  // first-member fast cast
  /* ... */
}
```

---

## FAQ

**Q: Why intrusive?**
A: No allocations, no key/value boxing, better cache locality, and you can overlay multiple indices over the same payload if you add multiple `macro_map_t` links.

**Q: What happens on duplicate insert into a unique map?**
A: The function returns `false` and the tree is unchanged.

**Q: Can I change the key while a node is in the tree?**
A: Don’t. Remove the node (`erase`), mutate the key, then insert again.

**Q: Is the API exception-safe / long-jmp safe?**
A: The code is plain C; it does not allocate and leaves the tree valid at every step of rebalancing.

---

## Test coverage

The repo’s tests exercise:

* unique & multimap inserts, duplicate handling
* `find` and full bounds matrix (`first/last/lower/upper/floor/ceiling`)
* forward/backward iterators and edge conditions
* erasing leaves / single-child / two-child nodes, repeatedly erasing current root
* copying trees (structure preserved, independence tested)
* randomized fuzz (unique and multimap)
* parent pointer/color packing and rotate guards

If you’re integrating into your project, copy the test patterns to validate your own comparators and payload types.

---

## License

SPDX-License-Identifier: Apache-2.0
Copyright © 2019-2025 Andy Curtis

Enjoy the speed and simplicity! If anything feels rough or surprising, open an issue or dive into the single header — it’s meant to be read.
