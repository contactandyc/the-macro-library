`macro_lru` provides **zero-allocation, intrusive** macros for maintaining doubly-linked lists. While they act as standard list primitives, their primary design goal is to power Least Recently Used (LRU) caches.

By operating directly on nodes that embed their own `next` and `prev` pointers, you avoid the overhead of allocating list containers.

---

## Quick start

### 1) Define your node struct

Your struct must contain `next` and `prev` pointers of the same type as the struct itself.

```c
typedef struct CacheNode {
    struct CacheNode *next;
    struct CacheNode *prev;
    
    char *key;
    void *data;
} CacheNode;

```

### 2) Initialize list heads and use the macros

Track your list using explicit `head` and `tail` pointers.

```c
CacheNode *head = NULL;
CacheNode *tail = NULL;

CacheNode node1 = {0};
CacheNode node2 = {0};

/* 1. Insert at the front (Most Recently Used) */
macro_dlist_push_front(head, tail, &node1);
macro_dlist_push_front(head, tail, &node2);

/* 2. Access an item and promote it to the front */
macro_lru_promote(head, tail, &node1);

/* 3. Evict the Least Recently Used item (from the back) */
CacheNode *evicted = NULL;
macro_dlist_pop_back(head, tail, evicted);
if (evicted) {
    printf("Evicted an item!\n");
}

/* 4. Remove a specific node manually */
macro_dlist_remove(head, tail, &node1);

```

---

## API surface

* **`macro_dlist_push_front(head, tail, node)`**
  Inserts `node` at the very beginning of the list. Updates `head` and `tail` pointers as necessary.
* **`macro_lru_promote(head, tail, node)`**
  Safely moves an existing `node` to the front of the list. If the node is already at the `head`, this does nothing. Ideal for marking an item as "recently used" upon access.
* **`macro_dlist_pop_back(head, tail, out_node)`**
  Removes the item at the `tail` (the least recently used item) and assigns it to `out_node`. It nullifies `out_node->next` and `out_node->prev` to safely detach it. If the list is empty, `out_node` is set to `NULL`.
* **`macro_dlist_remove(head, tail, node)`**
  Extracts an arbitrary `node` from the middle (or ends) of the list and wires the adjacent nodes together.

---

## Common pitfalls

* **Pointer Naming:** The macros implicitly expect your struct's internal pointers to be named exactly `next` and `prev`.
* **Detached state:** `macro_dlist_remove` does *not* `NULL` out the `next`/`prev` pointers of the removed node. If you plan to leave the node floating in memory, manually clear its pointers to avoid accidental traversals. (`macro_dlist_pop_back` handles this automatically).
* **Initialization:** Always ensure your `head` and `tail` pointers start as `NULL` before using `push_front`.
