// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>

#include "the-macro-library/macro_skiplist.h"
#include "the-macro-library/macro_test.h"

#define MAX_SKIPLIST_HEIGHT 12

/* ----- payload & comparators ----- */

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wflexible-array-extensions"
#endif

typedef struct Node {
    int k, v;
    /* Flexible array requires it to be the last member */
    macro_skiplist_t link;
} Node;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

static int node_cmp_noarg(const Node *a, const Node *b) {
    return (a->k > b->k) - (a->k < b->k);
}

static int kv_cmp_noarg(const int *key, const Node *n) {
    return (*key > n->k) - (*key < n->k);
}

/* ---------- Generate Skiplist Functions ---------- */

/* Unique insert and multimap insert */
macro_skiplist_insert_with_field(sl_insert, link, Node, node_cmp_noarg)
macro_multimap_skiplist_insert_with_field(sl_mmap_insert, link, Node, node_cmp_noarg)

/* Find / Bounds (Node vs Node) */
macro_skiplist_find_with_field(sl_find, link, Node, node_cmp_noarg)
macro_skiplist_lower_bound_with_field(sl_lower_bound, link, Node, node_cmp_noarg)

/* Find / Bounds (KV: int vs Node) */
macro_skiplist_find_kv_with_field(sl_find_kv, link, int, Node, kv_cmp_noarg)
macro_skiplist_lower_bound_kv_with_field(sl_lower_bound_kv, link, int, Node, kv_cmp_noarg)

macro_skiplist_upper_bound_kv_with_field(sl_upper_bound_kv, link, int, Node, kv_cmp_noarg)
macro_skiplist_floor_kv_with_field(sl_floor_kv, link, int, Node, kv_cmp_noarg)
macro_skiplist_ceiling_kv_with_field(sl_ceiling_kv, link, int, Node, kv_cmp_noarg)

/* ---------- Memory Helpers ---------- */

/* Helper to allocate a variable-height node */
static Node *new_node(int k, int v, uint8_t max_height) {
    uint8_t h = macro_skiplist_random_height(max_height);
    Node *n = (Node *)malloc(sizeof(Node) + (h * sizeof(void*)));
    n->k = k;
    n->v = v;
    n->link.height = h;
    for (int i = 0; i < h; i++) {
        MACRO_ATOMIC_INIT(&n->link.forward[i], NULL);
    }
    return n;
}

/* Helper to initialize the dummy head */
static macro_skiplist_t *new_head(uint8_t max_height) {
    macro_skiplist_t *head = (macro_skiplist_t *)malloc(sizeof(macro_skiplist_t) + (max_height * sizeof(void*)));
    macro_skiplist_init_head(head, max_height);
    return head;
}

/* Free the entire skiplist by walking level 0 */
static void free_skiplist(macro_skiplist_t *head) {
    macro_skiplist_t *curr = MACRO_ATOMIC_LOAD(&head->forward[0], acquire);
    while (curr) {
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&curr->forward[0], acquire);
        Node *n = macro_parent_object(curr, Node, link);
        free(n);
        curr = next;
    }
    free(head);
}

/* ---------- Validation Helpers ---------- */

/* Ensure level 0 is perfectly sorted */
static void assert_level0_sorted(macro_skiplist_t *head) {
    macro_skiplist_t *curr = MACRO_ATOMIC_LOAD(&head->forward[0], acquire);
    int lastk = INT_MIN;
    while (curr) {
        Node *nv = macro_parent_object(curr, Node, link);
        MACRO_ASSERT_TRUE(nv->k >= lastk);
        lastk = nv->k;
        curr = MACRO_ATOMIC_LOAD(&curr->forward[0], acquire);
    }
}

/* helper to build a shuffled array of keys */
static void shuffle(int *a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        size_t j = (size_t) (rand() % (int)(i + 1));
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

/* ---------- Tests ---------- */

MACRO_TEST(skiplist_unique_insert_and_find) {
    srand(101);
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);

    enum { N = 200 };
    int keys[N];
    for (int i = 0; i < N; ++i) { keys[i] = i; }
    shuffle(keys, N);

    /* unique inserts */
    for (int i = 0; i < N; ++i) {
        int k = keys[i];
        Node *n = new_node(k, k * 10, MAX_SKIPLIST_HEIGHT);
        bool ok = sl_insert(head, n);
        MACRO_ASSERT_TRUE(ok);
    }

    /* duplicate key should be rejected */
    Node *dup = new_node(42, 999, MAX_SKIPLIST_HEIGHT);
    bool okdup = sl_insert(head, dup);
    MACRO_ASSERT_TRUE(!okdup);
    free(dup);

    /* find existing / missing via KV */
    for (int k = -5; k <= N + 5; ++k) {
        Node *hit = sl_find_kv(head, &k);
        if (0 <= k && k < N) {
            MACRO_ASSERT_TRUE(hit && hit->k == k && hit->v == k * 10);
        } else {
            MACRO_ASSERT_TRUE(hit == NULL);
        }
    }

    assert_level0_sorted(head);
    free_skiplist(head);
}

MACRO_TEST(skiplist_multimap_duplicates_and_bounds) {
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);

    /* Insert elements with duplicate keys */
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(2, 10, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(2, 11, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(2, 12, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(4, 40, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(1,  1, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(5, 50, MAX_SKIPLIST_HEIGHT)));

    assert_level0_sorted(head);

    int k;

    /* Lower bound of 2 should hit the first '2' */
    k = 2;
    Node *lb = sl_lower_bound_kv(head, &k);
    MACRO_ASSERT_TRUE(lb && lb->k == 2);

    /* Lower bound of 3 should hit '4' */
    k = 3;
    lb = sl_lower_bound_kv(head, &k);
    MACRO_ASSERT_TRUE(lb && lb->k == 4);

    /* Lower bound of 6 should be NULL */
    k = 6;
    lb = sl_lower_bound_kv(head, &k);
    MACRO_ASSERT_TRUE(lb == NULL);

    /* Lower bound of 0 should hit '1' */
    k = 0;
    lb = sl_lower_bound_kv(head, &k);
    MACRO_ASSERT_TRUE(lb && lb->k == 1);

    free_skiplist(head);
}

MACRO_TEST(skiplist_extreme_keys_bounds) {
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);

    MACRO_ASSERT_TRUE(sl_insert(head, new_node(INT_MIN, 1, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_insert(head, new_node(0,       2, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_insert(head, new_node(INT_MAX, 3, MAX_SKIPLIST_HEIGHT)));

    assert_level0_sorted(head);

    int k;
    k = INT_MIN; MACRO_ASSERT_TRUE(sl_lower_bound_kv(head, &k)->k == INT_MIN);
    k = -1;      MACRO_ASSERT_TRUE(sl_lower_bound_kv(head, &k)->k == 0);
    k = 1;       MACRO_ASSERT_TRUE(sl_lower_bound_kv(head, &k)->k == INT_MAX);
    k = INT_MAX; MACRO_ASSERT_TRUE(sl_lower_bound_kv(head, &k)->k == INT_MAX);

    free_skiplist(head);
}

MACRO_TEST(skiplist_random_fuzz_order) {
    srand(888);
    for (int round = 0; round < 5; ++round) {
        macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);
        enum { N = 500 };

        /* random keys in range [0, 9999] */
        for (int i = 0; i < N; ++i) {
            int k = rand() % 10000;
            // Use mmap to freely allow duplicates
            sl_mmap_insert(head, new_node(k, i, MAX_SKIPLIST_HEIGHT));
        }

        assert_level0_sorted(head);

        /* Verify count */
        size_t count = 0;
        macro_skiplist_t *curr = MACRO_ATOMIC_LOAD(&head->forward[0], acquire);
        while (curr) {
            count++;
            curr = MACRO_ATOMIC_LOAD(&curr->forward[0], acquire);
        }
        MACRO_ASSERT_EQ_INT((int)count, N);

        free_skiplist(head);
    }
}

MACRO_TEST(skiplist_full_forward_walk) {
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);

    for (int i = 0; i < 50; ++i) {
        Node *n = new_node(i, i * 10, MAX_SKIPLIST_HEIGHT);
        MACRO_ASSERT_TRUE(sl_insert(head, n));
    }

    assert_level0_sorted(head);

    size_t cnt = 0;
    macro_skiplist_t *curr = MACRO_ATOMIC_LOAD(&head->forward[0], acquire);
    while (curr) {
        Node *nv = macro_parent_object(curr, Node, link);
        MACRO_ASSERT_EQ_INT(nv->k, (int)cnt);
        cnt++;
        curr = MACRO_ATOMIC_LOAD(&curr->forward[0], acquire);
    }
    MACRO_ASSERT_EQ_INT((int)cnt, 50);

    // Because we used heap allocation (new_node), we must free the list
    free_skiplist(head);
}

MACRO_TEST(skiplist_kv_vs_nonkv_parity) {
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);

    Node *n1 = new_node(4, 40, MAX_SKIPLIST_HEIGHT);
    Node *n2 = new_node(1, 10, MAX_SKIPLIST_HEIGHT);
    Node *n3 = new_node(9, 90, MAX_SKIPLIST_HEIGHT);
    Node *n4 = new_node(4, 44, MAX_SKIPLIST_HEIGHT);

    MACRO_ASSERT_TRUE(sl_insert(head, n1));
    MACRO_ASSERT_TRUE(sl_insert(head, n2));
    MACRO_ASSERT_TRUE(sl_insert(head, n3));
    MACRO_ASSERT_TRUE(!sl_insert(head, n4)); /* Unique map should reject duplicate 4 */
    free(n4); /* Must manually free rejected nodes in standard heap lifecycle */

    int k = 4;
    Node *a = sl_find_kv(head, &k);

    /* Dummy node for search is SAFE on the stack because we don't write to its pointers */
    Node key4;
    key4.k = 4;
    Node *b = sl_find(head, &key4);

    MACRO_ASSERT_TRUE(a == b && a && a->k == 4 && a->v == 40);

    free_skiplist(head);
}

MACRO_TEST(skiplist_multimap_stability) {
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);

    // Insert 3 items with identical keys
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(5, 100, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(5, 200, MAX_SKIPLIST_HEIGHT)));
    MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(5, 300, MAX_SKIPLIST_HEIGHT)));

    int k = 5;
    Node *first = sl_lower_bound_kv(head, &k);
    MACRO_ASSERT_TRUE(first != NULL);
    MACRO_ASSERT_EQ_INT(first->v, 100);

    macro_skiplist_t *next1 = MACRO_ATOMIC_LOAD(&first->link.forward[0], acquire);
    Node *second = macro_parent_object(next1, Node, link);
    MACRO_ASSERT_EQ_INT(second->v, 200);

    macro_skiplist_t *next2 = MACRO_ATOMIC_LOAD(&second->link.forward[0], acquire);
    Node *third = macro_parent_object(next2, Node, link);
    MACRO_ASSERT_EQ_INT(third->v, 300);

    free_skiplist(head);
}

MACRO_TEST(skiplist_bounds_full_matrix) {
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);
    int vals[] = {1, 2, 2, 2, 4, 6, 9};

    for (size_t i=0; i<sizeof(vals)/sizeof(vals[0]); ++i) {
        (void)sl_mmap_insert(head, new_node(vals[i], (int)i, MAX_SKIPLIST_HEIGHT));
    }

    int qs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (size_t i=0; i<sizeof(qs)/sizeof(qs[0]); ++i) {
        int k = qs[i];

        Node *lb = sl_lower_bound_kv(head, &k);
        Node *ub = sl_upper_bound_kv(head, &k);
        Node *fl = sl_floor_kv(head, &k);
        Node *ce = sl_ceiling_kv(head, &k);

        /* sanity relations */
        if (lb) { MACRO_ASSERT_TRUE(lb->k >= k); }
        if (ub) { MACRO_ASSERT_TRUE(ub->k >  k); }
        if (fl) { MACRO_ASSERT_TRUE(fl->k <= k); }
        if (ce) { MACRO_ASSERT_TRUE(ce->k >= k); }

        if (k == 2) {
            MACRO_ASSERT_TRUE(lb && lb->k == 2);
            MACRO_ASSERT_TRUE(ub && ub->k == 4);
            MACRO_ASSERT_TRUE(fl && fl->k == 2);
            MACRO_ASSERT_TRUE(ce && ce->k == 2);
        }
    }

    free_skiplist(head);
}

MACRO_TEST(skiplist_mmap_random_fuzz_with_dups) {
    srand(4242);
    macro_skiplist_t *head = new_head(MAX_SKIPLIST_HEIGHT);
    enum { N = 400 };

    for (int i=0; i<N; ++i) {
        int k = rand() % 40; /* many duplicates */
        MACRO_ASSERT_TRUE(sl_mmap_insert(head, new_node(k, i, MAX_SKIPLIST_HEIGHT)));
    }

    assert_level0_sorted(head);

    /* Lower/upper range should match contiguous block */
    for (int k=0; k<40; ++k) {
        Node *lb = sl_lower_bound_kv(head, &k);
        Node *ub = sl_upper_bound_kv(head, &k);
        if (!lb) {
            MACRO_ASSERT_TRUE(!ub || ub->k > k);
            continue;
        }

        /* Walk forward while equal to k; then we should meet ub */
        macro_skiplist_t *p = &lb->link;
        while (p && macro_parent_object(p, Node, link)->k == k) {
            p = MACRO_ATOMIC_LOAD(&p->forward[0], acquire);
        }

        if (ub) {
            MACRO_ASSERT_TRUE(p == &ub->link);
        } else {
            MACRO_ASSERT_TRUE(p == NULL);
        }
    }

    free_skiplist(head);
}

/* ----- Runner ----- */

int main(void) {
    macro_test_case tests[128];
    size_t test_count = 0;

    MACRO_ADD(tests, skiplist_unique_insert_and_find);
    MACRO_ADD(tests, skiplist_multimap_duplicates_and_bounds);
    MACRO_ADD(tests, skiplist_extreme_keys_bounds);
    MACRO_ADD(tests, skiplist_random_fuzz_order);
    MACRO_ADD(tests, skiplist_full_forward_walk);
    MACRO_ADD(tests, skiplist_kv_vs_nonkv_parity);
    MACRO_ADD(tests, skiplist_multimap_stability);
    MACRO_ADD(tests, skiplist_bounds_full_matrix);
    MACRO_ADD(tests, skiplist_mmap_random_fuzz_with_dups);
    macro_run_all("the-macro-library/macro_skiplist", tests, test_count);
    return 0;
}
