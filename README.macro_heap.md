# README.macro\_heap.md — header-only binary heap (min/max) generators

`macro_heap.h` generates **specialized**, **inline** min-heap or max-heap operations for your element type and comparator style (the same styles used by `macro_cmp.h`). It’s allocation-free: you manage the backing array; the generated functions just maintain the heap invariant.

## What does the heap look like?

You own a struct that carries the backing array and sizes:

```c
#include "the-macro-library/macro_heap.h"

typedef struct {
    int key, payload;
} Item;

/* Defines a POD heap handle with array + counters */
macro_heap_t(ItemHeap, Item);  // expands to: typedef struct { Item* arr; size_t num, size; } ItemHeap;
```

* `arr`  — pointer to capacity `size` elements you allocated.
* `num`  — current heap size (number of valid items).
* `size` — current capacity.

> The library **never** reallocates. You must ensure capacity before pushes/replacements.

---

## Operations you can generate

There are **min** and **max** flavors, and for each you can either

* **bake** a comparator at generation time, or
* accept a comparator (and optional `arg`) **at call site**.

All functions are generated for a **name** you choose.

### Push

> **Contract**: Put the new element at `ht->arr[ht->num]` yourself, ensure capacity, then call push. After the call, `ht->num` is incremented and the heap property is restored.

* Bake comparator:

  ```c
  macro_min_heap_push(name, style, type, cmp, heap_type)
  macro_max_heap_push(name, style, type, cmp, heap_type)
  ```

* Comparator at call site:

  ```c
  macro_min_heap_push_compare(name, style, type, heap_type)
  macro_max_heap_push_compare(name, style, type, heap_type)
  ```

### Pop

> **Contract**: Removes the top element (index 0) and returns a pointer to it **moved to** `ht->arr[ht->num]`. After the call, `ht->num` has been decremented. The returned pointer remains valid until the next modifying operation.

* Bake comparator:

  ```c
  macro_min_heap_pop(name, style, type, cmp, heap_type)
  macro_max_heap_pop(name, style, type, cmp, heap_type)
  ```

* Comparator at call site:

  ```c
  macro_min_heap_pop_compare(name, style, type, heap_type)
  macro_max_heap_pop_compare(name, style, type, heap_type)
  ```

### Replace (replace the top element with a new one)

> **Typical use**: You want to pop the current top **and** insert a new value in one shot.
> **Contract**: Write the replacement value, then call replace. Internally the code ensures the heap property (in the implementation this is done via a pop+push pathway, with checks to avoid work when already ordered).

* Bake comparator:

  ```c
  macro_min_heap_replace(name, style, type, cmp, heap_type)
  macro_max_heap_replace(name, style, type, cmp, heap_type)
  ```

* Comparator at call site:

  ```c
  macro_min_heap_replace_compare(name, style, type, heap_type)
  macro_max_heap_replace_compare(name, style, type, heap_type)
  ```

> Note on where to put the replacement value: the intended API is “replace the root”. In practice, you either:
>
> * put the new value at `ht->arr[0]` and call `replace()`, or
> * follow a pop-then-push pattern yourself (see examples).
>
> Both achieve the same result; use whichever matches your workflow.

---

## Comparator “styles” (from `macro_cmp.h`)

Choose one of the supported styles (same as the rest of the library):

* `cmp_no_arg` : `int cmp(const T*, const T*)`
* `cmp_arg`    : `int cmp(const T*, const T*, void *arg)`
* `arg_cmp`    : `int cmp(void *arg, const T*, const T*)`
* (There are also “less” styles, but heaps almost always use `cmp_*`.)

The `*_compare` variants accept a comparator (and optional `arg`) at call-time; the others **bake** a named comparator into the generated function.

---

## Minimal, complete example (min-heap of `Item` by `key`)

```c
#include <stdio.h>
#include <stdlib.h>
#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_heap.h"

typedef struct { int key, payload; } Item;
macro_heap_t(ItemHeap, Item);

static int item_cmp(const Item *a, const Item *b) {
  return (a->key > b->key) - (a->key < b->key);
}

/* Generate 3 functions bound to our comparator and types */
macro_min_heap_push   (heap_push_item,   cmp_no_arg, Item, item_cmp, ItemHeap);
macro_min_heap_pop    (heap_pop_item,    cmp_no_arg, Item, item_cmp, ItemHeap);
macro_min_heap_replace(heap_replace_item,cmp_no_arg, Item, item_cmp, ItemHeap);

int main(void) {
  size_t cap = 16;
  Item *buf = malloc(cap * sizeof *buf);
  ItemHeap h = { .arr = buf, .num = 0, .size = cap };

  /* push three items */
  Item x = {3,30}; h.arr[h.num] = x; heap_push_item(&h);
  Item y = {1,10}; h.arr[h.num] = y; heap_push_item(&h);
  Item z = {5,50}; h.arr[h.num] = z; heap_push_item(&h);

  /* peek top (min) at index 0 */
  printf("min=%d\n", h.arr[0].key); // -> 1

  /* pop top */
  Item *p = heap_pop_item(&h);      // returns pointer at arr[h.num] (previous top)
  printf("popped=%d\n", p->key);    // -> 1

  /* replace top with a new value (two idioms): */

  /* Idiom A: write to root then replace */
  h.arr[0] = (Item){ 4, 40 };
  heap_replace_item(&h);

  /* Idiom B: pop, write new at end, push (equivalent outcome) */
  (void)heap_pop_item(&h);                  // remove current top
  h.arr[h.num] = (Item){ 2, 20 };           // place new one “after the array”
  heap_push_item(&h);                       // sift it into place

  free(buf);
}
```

---

## Using the `*_compare` variants (comparator at call-time)

```c
/* Generate min-heap push/pop that accept a comparator pointer */
macro_min_heap_push_compare(push_cmp,   cmp_no_arg, Item, ItemHeap);
macro_min_heap_pop_compare (pop_cmp,    cmp_no_arg, Item, ItemHeap);
macro_min_heap_replace_compare(repl_cmp,cmp_no_arg, Item, ItemHeap);

ItemHeap h = ...;
int (*cmp)(const Item*, const Item*) = item_cmp;

h.arr[h.num] = (Item){7,70};
push_cmp(&h, cmp);        // pass comparator at call site
Item *out = pop_cmp(&h, cmp);
```

For `cmp_arg` / `arg_cmp`, the generated signatures also include `void *arg`.

---

## Common patterns & tips

* **Capacity**: Always check `ht->num < ht->size` before push/replace. The library does not grow the buffer.

* **Push**: Copy the new value to `arr[num]` **first**, then call your push function. That is by design and avoids an extra move.

* **Pop**: After popping, the removed element is stored at `arr[num]` and a pointer to it is returned. Copy it out **before** any further push/pop/replace (which may overwrite that slot).

* **Replace**:

    * If you already have the new value in hand and don’t care about the old root, idiom **A** is shortest:

      ```c
      h.arr[0] = new_value;
      heap_replace(&h);   // restores heap invariant for new root
      ```
    * If you also need the old value, idiom **B** (pop then push) makes that explicit (see example).

* **Min vs Max**: Simply choose the corresponding macro family. You can generate both for the same `heap_type` under different names.

* **Stability**: Heaps do not preserve input order among equals.

* **Complexity**: Push, pop, and replace are **O(log n)**. Accessing the top (root) is **O(1)** (`ht->arr[0]`).

* **Thread safety**: The functions are re-entrant and thread-safe as long as each heap instance is confined to one thread at a time.

---

## API cheat sheet

```c
/* Heap handle */
macro_heap_t(HeapName, T)  // typedef struct { T* arr; size_t num, size; } HeapName;

/* Min heap (baked-in comparator) */
macro_min_heap_push   (name, style, T, cmp, HeapName)
macro_min_heap_pop    (name, style, T, cmp, HeapName)       // returns T*
macro_min_heap_replace(name, style, T, cmp, HeapName)

/* Max heap (baked-in comparator) */
macro_max_heap_push   (name, style, T, cmp, HeapName)
macro_max_heap_pop    (name, style, T, cmp, HeapName)       // returns T*
macro_max_heap_replace(name, style, T, cmp, HeapName)

/* Min heap (comparator at call site) */
macro_min_heap_push_compare   (name, style, T, HeapName)
macro_min_heap_pop_compare    (name, style, T, HeapName)    // returns T*
macro_min_heap_replace_compare(name, style, T, HeapName)

/* Max heap (comparator at call site) */
macro_max_heap_push_compare   (name, style, T, HeapName)
macro_max_heap_pop_compare    (name, style, T, HeapName)    // returns T*
macro_max_heap_replace_compare(name, style, T, HeapName)
```

Where `style` is one of the comparator styles defined by `macro_cmp.h` (typically `cmp_no_arg`, `cmp_arg`, or `arg_cmp`), and `cmp` is the comparator function name when you bake it in.

---

## Gotchas

* **Don’t forget to write the element before pushing** (to `arr[num]`). Push assumes it’s already there.
* The pointer returned by **pop** refers to `arr[num]` **after** the pop. It can be clobbered by the next modifying operation.
* Heaps expect a **strict weak ordering**: make sure your comparator implements it consistently with your chosen min/max direction.
* This header includes internal helpers from `src/macro_heap_sort.h`. Include paths must be set correctly.

---

## License

SPDX-License-Identifier: Apache-2.0
Copyright © 2019–2025 Andy Curtis
