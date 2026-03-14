// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>

#include "the-macro-library/macro_map.h"
#include "the-macro-library/macro_test.h"

/* ----- payload & comparators ----- */

typedef struct Node {
    int k, v;
    macro_map_t link;
} Node;

static int node_cmp_noarg(const Node *a, const Node *b) {
    return (a->k > b->k) - (a->k < b->k);
}
static int kv_cmp_noarg(const int *key, const Node *n) {
    return (*key > n->k) - (*key < n->k);
}

/* ---------- Extra helpers ---------- */
static void assert_root_invariants(macro_map_t *root) {
    if (!root) return;
    MACRO_ASSERT_TRUE(_macro_map_parent(root) == NULL);
    MACRO_ASSERT_TRUE(_macro_map_is_black(root));
}

static void assert_inorder_sorted(macro_map_t *root) {
    macro_map_t *it = macro_map_first(root);
    int lastk = INT_MIN;
    while (it) {
        Node *nv = macro_parent_object(it, Node, link);
        MACRO_ASSERT_TRUE(nv->k >= lastk);
        lastk = nv->k;
        it = macro_map_next(it);
    }
}

/* Collect inorder keys into an array for reference comparisons */
static size_t collect_inorder(macro_map_t *root, int *out, size_t cap) {
    size_t c = 0;
    for (macro_map_t *p = macro_map_first(root); p && c < cap; p = macro_map_next(p)) {
        Node *nv = macro_parent_object(p, Node, link);
        out[c++] = nv->k;
    }
    return c;
}

/* Build a reference sorted array of ints (with duplicates allowed) */
static int int_cmp(const void *a, const void *b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x>y) - (x<y);
}


/* ----- generate functions (with-field) ----- */

/* unique and multi insert */
macro_map_insert_with_field     (map_insert,  link, Node, node_cmp_noarg)
macro_multimap_insert_with_field(mmap_insert, link, Node, node_cmp_noarg)

/* find (kv) */
macro_map_find_kv_with_field(map_find_kv, link, int, Node, kv_cmp_noarg)

/* bounds (kv) via internal helpers to test all flavors */
_macro_map_kv_with_field(map_find_first,     link, first,       macro_map_default(), int, Node, kv_cmp_noarg)
_macro_map_kv_with_field(map_find_last,      link, last,        macro_map_default(), int, Node, kv_cmp_noarg)
_macro_map_kv_with_field(map_lower_bound_kv, link, lower_bound, macro_map_default(), int, Node, kv_cmp_noarg)
_macro_map_kv_with_field(map_upper_bound_kv, link, upper_bound, macro_map_default(), int, Node, kv_cmp_noarg)
_macro_map_kv_with_field(map_floor_kv,       link, floor,       macro_map_default(), int, Node, kv_cmp_noarg)
_macro_map_kv_with_field(map_ceiling_kv,     link, ceiling,     macro_map_default(), int, Node, kv_cmp_noarg)

/* ----- RB invariants check ----- */

static void check_invariants(macro_map_t *root) {
    if (!root) return;

    /* root must be black */
    MACRO_ASSERT_TRUE(_macro_map_is_black(root));

    /* check black-height & red-parent rule */
    int expected = -1;

    /* simple DFS */
    struct Frame { macro_map_t *n; int blacks; } st[256];
    int sp = 0;
    st[sp++] = (struct Frame){ root, _macro_map_is_black(root) ? 1 : 0 };

    while (sp) {
        struct Frame f = st[--sp];
        macro_map_t *n = f.n;

        /* red node must not have red child */
        if (_macro_map_is_red(n)) {
            MACRO_ASSERT_TRUE(!n->left  || _macro_map_is_black(n->left));
            MACRO_ASSERT_TRUE(!n->right || _macro_map_is_black(n->right));
        }

        /* parent pointers must be consistent */
        if (n->left)  MACRO_ASSERT_TRUE(_macro_map_parent(n->left)  == n);
        if (n->right) MACRO_ASSERT_TRUE(_macro_map_parent(n->right) == n);

        if (!n->left && !n->right) {
            /* leaf: finalize black count */
            int bh = f.blacks;
            if (expected == -1) expected = bh;
            MACRO_ASSERT_EQ_INT(expected, bh);
        } else {
            if (n->left)  st[sp++] = (struct Frame){ n->left,  f.blacks + (_macro_map_is_black(n->left)?1:0) };
            if (n->right) st[sp++] = (struct Frame){ n->right, f.blacks + (_macro_map_is_black(n->right)?1:0) };
        }
    }

    /* in-order must be nondecreasing */
    macro_map_t *it = macro_map_first(root);
    int lastk = INT32_MIN;
    while (it) {
        Node *nv = macro_parent_object(it, Node, link);
        MACRO_ASSERT_TRUE(nv->k >= lastk);
        lastk = nv->k;
        it = macro_map_next(it);
    }
}

/* helper to build a shuffled array of keys */
static void shuffle(int *a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        size_t j = (size_t) (rand() % (int)(i + 1));
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

/* ----- Tests ----- */

MACRO_TEST(map_unique_insert_find_iter) {
    srand(12345);
    enum { N = 200 };
    Node nodes[N];
    int keys[N];
    for (int i = 0; i < N; ++i) { keys[i] = i; }
    shuffle(keys, N);

    macro_map_t *root = NULL;

    /* unique inserts */
    for (int i = 0; i < N; ++i) {
        int k = keys[i];
        nodes[k].k = k; nodes[k].v = k * 10;
        bool ok = map_insert(&root, &nodes[k]);
        MACRO_ASSERT_TRUE(ok);
    }

    /* duplicate key should be rejected (same content, different node) */
    Node dup = { .k = 42, .v = 999 };
    bool okdup = map_insert(&root, &dup);
    MACRO_ASSERT_TRUE(!okdup);

    /* find existing / missing */
    for (int k = -5; k <= N + 5; ++k) {
        Node *hit = map_find_kv(root, &k);
        if (0 <= k && k < N) {
            MACRO_ASSERT_TRUE(hit && hit->k == k);
        } else {
            MACRO_ASSERT_TRUE(hit == NULL);
        }
    }

    /* iterators and invariants */
    check_invariants(root);

    /* in-order is increasing */
    macro_map_t *it = macro_map_first(root);
    int expect = 0;
    while (it) {
        Node *nv = macro_parent_object(it, Node, link);
        MACRO_ASSERT_EQ_INT(expect++, nv->k);
        it = macro_map_next(it);
    }
    MACRO_ASSERT_EQ_INT(expect, N);
}

MACRO_TEST(map_bounds_and_edges) {
    macro_map_t *root = NULL;
    int vals[] = {1,2,2,2,4,5,7};
    Node nodes[sizeof(vals)/sizeof(vals[0])];

    for (size_t i = 0; i < sizeof(vals)/sizeof(vals[0]); ++i) {
        nodes[i].k = vals[i]; nodes[i].v = (int)i;
        bool ok = map_insert(&root, &nodes[i]);
        MACRO_ASSERT_TRUE(ok || vals[i]==2); /* unique map: only one '2' accepted */
    }

    int k;
    k = 0; MACRO_ASSERT_TRUE(map_lower_bound_kv(root, &k) == &nodes[0]);
    k = 2; MACRO_ASSERT_TRUE(map_lower_bound_kv(root, &k) && map_lower_bound_kv(root, &k)->k == 2);
    k = 3; MACRO_ASSERT_TRUE(map_lower_bound_kv(root, &k) && map_lower_bound_kv(root, &k)->k == 4);
    k = 6; MACRO_ASSERT_TRUE(map_lower_bound_kv(root, &k) && map_lower_bound_kv(root, &k)->k == 7);
    k = 8; MACRO_ASSERT_TRUE(map_lower_bound_kv(root, &k) == NULL);

    k = 2; MACRO_ASSERT_TRUE(map_upper_bound_kv(root, &k) && map_upper_bound_kv(root, &k)->k == 4);
    k = 5; MACRO_ASSERT_TRUE(map_upper_bound_kv(root, &k) && map_upper_bound_kv(root, &k)->k == 7);
    k = 7; MACRO_ASSERT_TRUE(map_upper_bound_kv(root, &k) == NULL);

    k = 0; MACRO_ASSERT_TRUE(map_floor_kv(root, &k) == NULL);
    k = 3; MACRO_ASSERT_TRUE(map_floor_kv(root, &k) && map_floor_kv(root, &k)->k == 2);
    k = 8; MACRO_ASSERT_TRUE(map_floor_kv(root, &k) && map_floor_kv(root, &k)->k == 7);

    k = 0; MACRO_ASSERT_TRUE(map_ceiling_kv(root, &k) && map_ceiling_kv(root, &k)->k == 1);
    k = 3; MACRO_ASSERT_TRUE(map_ceiling_kv(root, &k) && map_ceiling_kv(root, &k)->k == 4);
    k = 8; MACRO_ASSERT_TRUE(map_ceiling_kv(root, &k) == NULL);

    check_invariants(root);
}

MACRO_TEST(map_erase_leaf_one_two_children) {
    macro_map_t *root = NULL;
    enum { N = 64 };
    Node nodes[N];

    /* Insert [0..63] */
    for (int i = 0; i < N; ++i) {
        nodes[i].k = i; nodes[i].v = i*3;
        MACRO_ASSERT_TRUE(map_insert(&root, &nodes[i]));
    }
    check_invariants(root);

    /* erase a leaf, a one-child, and a two-children node */
    int targets[] = { 1, 2, 31, 32, 63, 0, 17, 25, 40 };
    for (size_t i = 0; i < sizeof(targets)/sizeof(targets[0]); ++i) {
        int k = targets[i];
        Node *n = map_find_kv(root, &k);
        MACRO_ASSERT_TRUE(n != NULL);
        MACRO_ASSERT_TRUE(macro_map_erase(&root, &n->link));
        MACRO_ASSERT_TRUE(map_find_kv(root, &k) == NULL);
        check_invariants(root);
    }
}

MACRO_TEST(multimap_duplicates_and_bounds) {
    macro_map_t *root = NULL;
    Node a = {.k=2,.v=10}, b={.k=2,.v=11}, c={.k=2,.v=12}, d={.k=4,.v=40};
    Node e = {.k=1,.v=1},  f={.k=5,.v=50};

    MACRO_ASSERT_TRUE(mmap_insert(&root, &a));
    MACRO_ASSERT_TRUE(mmap_insert(&root, &b));
    MACRO_ASSERT_TRUE(mmap_insert(&root, &c));
    MACRO_ASSERT_TRUE(mmap_insert(&root, &d));
    MACRO_ASSERT_TRUE(mmap_insert(&root, &e));
    MACRO_ASSERT_TRUE(mmap_insert(&root, &f));

    int k = 2;
    Node *first2 = map_find_first(root, &k);
    Node *last2  = map_find_last (root, &k);
    MACRO_ASSERT_TRUE(first2 && last2);
    MACRO_ASSERT_TRUE(first2->k == 2 && last2->k == 2);

    /* lower/upper span the 2's, floor/ceiling around gaps */
    k = 3; MACRO_ASSERT_TRUE(map_lower_bound_kv(root, &k) && map_lower_bound_kv(root, &k)->k == 4);
    k = 3; MACRO_ASSERT_TRUE(map_floor_kv(root, &k) && map_floor_kv(root, &k)->k == 2);
    k = 0; MACRO_ASSERT_TRUE(map_ceiling_kv(root, &k) && map_ceiling_kv(root, &k)->k == 1);
    k = 6; MACRO_ASSERT_TRUE(map_floor_kv(root, &k) && map_floor_kv(root, &k)->k == 5);

    /* postorder traversal: visit all nodes */
    size_t seen = 0;
    for (macro_map_t *p = macro_map_postorder_first(root); p; p = macro_map_postorder_next(p)) {
        ++seen;
    }
    MACRO_ASSERT_EQ_INT(seen, 6);

    check_invariants(root);
}

static macro_map_t *copy_cb_impl(macro_map_t *lnk, void *arg) {
    (void)arg;
    Node *orig = macro_parent_object(lnk, Node, link);
    Node *nw = (Node *)malloc(sizeof(*nw));
    *nw = *orig;  /* left/right/parent set by copier */
    return &nw->link;
}

MACRO_TEST(map_copy_and_independence) {
    macro_map_t *root = NULL;
    Node n1={.k=10,.v=1}, n2={.k=5,.v=2}, n3={.k=15,.v=3};
    MACRO_ASSERT_TRUE(map_insert(&root, &n1));
    MACRO_ASSERT_TRUE(map_insert(&root, &n2));
    MACRO_ASSERT_TRUE(map_insert(&root, &n3));

    macro_map_t *copy = macro_map_copy(root, copy_cb_impl, NULL);
    check_invariants(root);
    check_invariants(copy);

    int k=10;
    Node *a = map_find_kv(root, &k);
    Node *b = map_find_kv(copy, &k);
    MACRO_ASSERT_TRUE(a && b && a != b && a->k == b->k);

    /* Erase from copy doesn't affect original */
    MACRO_ASSERT_TRUE(macro_map_erase(&copy, &b->link));
    MACRO_ASSERT_TRUE(map_find_kv(copy, &k) == NULL);
    MACRO_ASSERT_TRUE(map_find_kv(root, &k) != NULL);

    /* cleanup: free copied nodes in postorder */
    for (macro_map_t *p = macro_map_postorder_first(copy); p; ) {
        macro_map_t *next = macro_map_postorder_next(p);
        Node *nv = macro_parent_object(p, Node, link);
        free(nv);
        p = next;
    }
}

MACRO_TEST(map_iterator_edges) {
    macro_map_t *root = NULL;
    Node a={.k=10,.v=1}, b={.k=5,.v=2}, c={.k=15,.v=3};
    MACRO_ASSERT_TRUE(map_insert(&root, &a));
    MACRO_ASSERT_TRUE(map_insert(&root, &b));
    MACRO_ASSERT_TRUE(map_insert(&root, &c));
    check_invariants(root);

    macro_map_t *f = macro_map_first(root);
    macro_map_t *l = macro_map_last(root);
    MACRO_ASSERT_TRUE(f && l && f != l);

    /* first->prev == NULL, last->next == NULL */
    MACRO_ASSERT_TRUE(macro_map_previous(f) == NULL);
    MACRO_ASSERT_TRUE(macro_map_next(l) == NULL);

    /* walk forward and backward */
    macro_map_t *m = macro_map_next(f);
    MACRO_ASSERT_TRUE(m && macro_map_previous(m) == f);
    MACRO_ASSERT_TRUE(macro_map_next(m) == l);
    MACRO_ASSERT_TRUE(macro_map_previous(l) == m);

    assert_inorder_sorted(root);
}


MACRO_TEST(map_bounds_full_matrix) {
    macro_map_t *root = NULL;
    int vals[] = {1,2,2,2,4,6,9};
    Node nodes[sizeof(vals)/sizeof(vals[0])];

    for (size_t i=0;i<sizeof(vals)/sizeof(vals[0]);++i) {
        nodes[i].k = vals[i]; nodes[i].v = (int)i;
        (void)mmap_insert(&root, &nodes[i]); /* multimap: accept all 2's */
    }
    check_invariants(root);

    int qs[] = {0,1,2,3,4,5,6,7,8,9,10};
    for (size_t i=0;i<sizeof(qs)/sizeof(qs[0]);++i) {
        int k = qs[i];

        Node *lb = map_lower_bound_kv(root, &k);
        Node *ub = map_upper_bound_kv(root, &k);
        Node *fl = map_floor_kv(root, &k);
        Node *ce = map_ceiling_kv(root, &k);

        /* sanity relations */
        if (lb) { MACRO_ASSERT_TRUE(lb->k >= k); }
        if (ub) { MACRO_ASSERT_TRUE(ub->k >  k); }
        if (fl) { MACRO_ASSERT_TRUE(fl->k <= k); }
        if (ce) { MACRO_ASSERT_TRUE(ce->k >= k); }

        if (k==2) {
            MACRO_ASSERT_TRUE(lb && lb->k==2);
            MACRO_ASSERT_TRUE(ub && ub->k==4);
            MACRO_ASSERT_TRUE(fl && fl->k==2);
            MACRO_ASSERT_TRUE(ce && ce->k==2);
        }
    }
}


MACRO_TEST(map_erase_root_and_shapes) {
    macro_map_t *root = NULL;
    enum { N = 100 };
    Node nodes[N];

    for (int i=0;i<N;++i) {
        nodes[i].k=i; nodes[i].v=i;
        MACRO_ASSERT_TRUE(map_insert(&root, &nodes[i]));
    }
    check_invariants(root);

    /* erase the current root 10 times */
    for (int rep=0; rep<10; ++rep) {
        macro_map_t *r = macro_map_first(root);
        while (_macro_map_parent(r)) r = _macro_map_parent(r);
        Node *rn = macro_parent_object(r, Node, link);
        MACRO_ASSERT_TRUE(macro_map_erase(&root, &rn->link));
        check_invariants(root);
    }

    /* collect whatever keys are *currently* present */
    int present[N];
    size_t pn = 0;
    for (macro_map_t *p = macro_map_first(root); p; p = macro_map_next(p)) {
        Node *nv = macro_parent_object(p, Node, link);
        present[pn++] = nv->k;
    }

    /* delete them all in a random order */
    shuffle(present, pn);
    for (size_t i=0; i<pn; ++i) {
        int k = present[i];
        Node *n = map_find_kv(root, &k);
        MACRO_ASSERT_TRUE(n != NULL);
        MACRO_ASSERT_TRUE(macro_map_erase(&root, &n->link));
        check_invariants(root);
    }

    MACRO_ASSERT_TRUE(root == NULL);
}

/* helpers to find a non-root node with a particular child (if any) */
static macro_map_t *find_nonroot_with_right_child(macro_map_t *root) {
    for (macro_map_t *p = macro_map_first(root); p; p = macro_map_next(p))
        if (_macro_map_parent(p) && p->right) return p;
    return NULL;
}
static macro_map_t *find_nonroot_with_left_child(macro_map_t *root) {
    for (macro_map_t *p = macro_map_first(root); p; p = macro_map_next(p))
        if (_macro_map_parent(p) && p->left) return p;
    return NULL;
}

/* sanity for parent links only (not color invariants) */
static void assert_parent_links_ok(macro_map_t *root) {
    for (macro_map_t *p = macro_map_first(root); p; p = macro_map_next(p)) {
        macro_map_t *par = _macro_map_parent(p);
        if (par) {
            MACRO_ASSERT_TRUE(par->left == p || par->right == p);
            MACRO_ASSERT_TRUE(p != par);
        }
        if (p->left)  MACRO_ASSERT_TRUE(_macro_map_parent(p->left)  == p);
        if (p->right) MACRO_ASSERT_TRUE(_macro_map_parent(p->right) == p);
    }
}

MACRO_TEST(map_rotate_guard_root_null) {
    macro_map_t *root = NULL;
    /* build something with interior nodes that have both children */
    Node a={.k=10}, b={.k=5}, c={.k=15}, d={.k=12}, e={.k=18}, f={.k=3}, g={.k=7};
    MACRO_ASSERT_TRUE(map_insert(&root, &a));
    MACRO_ASSERT_TRUE(map_insert(&root, &b));
    MACRO_ASSERT_TRUE(map_insert(&root, &c));
    MACRO_ASSERT_TRUE(map_insert(&root, &d));
    MACRO_ASSERT_TRUE(map_insert(&root, &e));
    MACRO_ASSERT_TRUE(map_insert(&root, &f));
    MACRO_ASSERT_TRUE(map_insert(&root, &g));
    check_invariants(root); /* clean baseline */

    /* 1) Left rotate then right rotate back (root==NULL in both calls) */
    macro_map_t *p = find_nonroot_with_right_child(root);
    if (p) {
        macro_map_t *orig_parent = _macro_map_parent(p);
        macro_map_t *orig_right  = p->right;

        _macro_map_rotate_left(p, NULL);                /* mutate locally */
        /* After a left rotate at p, orig_right becomes p’s parent */
        MACRO_ASSERT_TRUE(_macro_map_parent(p) == orig_right);
        assert_parent_links_ok(root);                   /* still a sane BST */

        _macro_map_rotate_right(_macro_map_parent(p), NULL); /* inverse */
        /* back to original relationships */
        MACRO_ASSERT_TRUE(p->right == orig_right);
        MACRO_ASSERT_TRUE(_macro_map_parent(p) == orig_parent);
        assert_parent_links_ok(root);
    }

    /* 2) Right rotate then left rotate back */
    macro_map_t *q = find_nonroot_with_left_child(root);
    if (q) {
        macro_map_t *orig_parent = _macro_map_parent(q);
        macro_map_t *orig_left   = q->left;

        _macro_map_rotate_right(q, NULL);
        MACRO_ASSERT_TRUE(_macro_map_parent(q) == orig_left);
        assert_parent_links_ok(root);

        _macro_map_rotate_left(_macro_map_parent(q), NULL);
        MACRO_ASSERT_TRUE(q->left == orig_left);
        MACRO_ASSERT_TRUE(_macro_map_parent(q) == orig_parent);
        assert_parent_links_ok(root);
    }

    /* Now the tree is restored exactly; full RB invariants should hold. */
    check_invariants(root);
}



MACRO_TEST(map_parent_color_packing) {
    macro_map_t *root=NULL;
    Node a={.k=10,.v=0}, b={.k=5,.v=0}, c={.k=15,.v=0};
    MACRO_ASSERT_TRUE(map_insert(&root, &a));
    MACRO_ASSERT_TRUE(map_insert(&root, &b));
    MACRO_ASSERT_TRUE(map_insert(&root, &c));
    check_invariants(root);

    /* Every node’s parent pointer recovered from parent_color must match fields */
    for (macro_map_t *p=macro_map_first(root); p; p=macro_map_next(p)) {
        macro_map_t *parent = _macro_map_parent(p);
        if (parent) {
            MACRO_ASSERT_TRUE(parent->left == p || parent->right == p);
        }
        /* color bit only uses LSB */
        uintptr_t pc = p->parent_color;
        MACRO_ASSERT_TRUE((pc & (uintptr_t)1) == 0 || (pc & (uintptr_t)1) == 1);
    }
}


MACRO_TEST(map_random_fuzz_unique_vs_vector) {
    srand(777);
    for (int round=0; round<10; ++round) {
        macro_map_t *root=NULL;
        enum { N = 300 };
        Node *nodes = (Node*)calloc(N, sizeof(Node));
        bool used[2000]={0};
        int ref[ N ];
        size_t refn=0;

        /* random unique keys in range [0,1999] */
        for (int i=0;i<N;++i) {
            int k;
            do { k = rand()%2000; } while (used[k]);
            used[k]=true;
            nodes[i].k=k; nodes[i].v=i;
            MACRO_ASSERT_TRUE(map_insert(&root, &nodes[i]));
            ref[refn++]=k;
            if ((i%17)==0) check_invariants(root);
        }
        check_invariants(root);

        qsort(ref, (size_t)refn, sizeof(ref[0]), int_cmp);

        int got[N];
        size_t gn = collect_inorder(root, got, N);
        MACRO_ASSERT_EQ_INT((int)gn, (int)refn);
        for (size_t i=0;i<gn;++i) MACRO_ASSERT_EQ_INT(ref[i], got[i]);

        /* Randomly erase half */
        for (int i=0;i<N;i+=2) {
            Node *hit = map_find_kv(root, &nodes[i].k);
            if (hit) {
                MACRO_ASSERT_TRUE(macro_map_erase(&root, &hit->link));
                if ((i%19)==0) check_invariants(root);
            }
        }
        check_invariants(root);

        free(nodes);
        /* optional: free tree not needed; nodes are stack/heap local */
    }
}


MACRO_TEST(mmap_random_fuzz_with_dups) {
    srand(4242);
    macro_map_t *root=NULL;
    enum { N = 400 };
    Node *nodes = (Node*)calloc(N, sizeof(Node));
    for (int i=0;i<N;++i) {
        nodes[i].k = rand()%40; /* many duplicates */
        nodes[i].v = i;
        MACRO_ASSERT_TRUE(mmap_insert(&root, &nodes[i]));
        if ((i%23)==0) check_invariants(root);
    }
    check_invariants(root);

    /* Lower/upper range should match contiguous block in inorder */
    for (int k=0;k<40;++k) {
        Node *lb = map_lower_bound_kv(root, &k);
        Node *ub = map_upper_bound_kv(root, &k);
        if (!lb) {
            MACRO_ASSERT_TRUE(!ub || ub->k > k);
            continue;
        }
        /* Walk forward while equal to k; then we should meet ub */
        macro_map_t *p = &lb->link;
        while (p && macro_parent_object(p, Node, link)->k == k) p = macro_map_next(p);
        if (ub) MACRO_ASSERT_TRUE(p == &ub->link); else MACRO_ASSERT_TRUE(p == NULL);
    }

    /* Erase all nodes with an odd key */
    for (int k=1; k<40; k+=2) {
        for (;;) {
            Node *lb = map_lower_bound_kv(root, &k);
            if (!lb || lb->k != k) break;
            MACRO_ASSERT_TRUE(macro_map_erase(&root, &lb->link));
            check_invariants(root);
        }
    }
    check_invariants(root);
    free(nodes);
}


MACRO_TEST(map_copy_large) {
    macro_map_t *root=NULL;
    enum { N=256 };
    Node *nodes = (Node*)calloc(N, sizeof(Node));
    for (int i=0;i<N;++i) {
        nodes[i].k = (i*73) % 997; nodes[i].v = i;
        MACRO_ASSERT_TRUE(map_insert(&root, &nodes[i]));
    }
    check_invariants(root);

    macro_map_t *cpy = macro_map_copy(root, copy_cb_impl, NULL);
    check_invariants(cpy);

    /* structures are independent */
    int k = nodes[123].k;
    Node *orig = map_find_kv(root, &k);
    Node *dup  = map_find_kv(cpy,  &k);
    MACRO_ASSERT_TRUE(orig && dup && orig!=dup);

    MACRO_ASSERT_TRUE(macro_map_erase(&cpy, &dup->link));
    MACRO_ASSERT_TRUE(map_find_kv(cpy, &k) == NULL);
    MACRO_ASSERT_TRUE(map_find_kv(root, &k) != NULL);

    /* cleanup copy */
    for (macro_map_t *p = macro_map_postorder_first(cpy); p; ) {
        macro_map_t *nxt = macro_map_postorder_next(p);
        Node *nv = macro_parent_object(p, Node, link);
        free(nv);
        p = nxt;
    }
    free(nodes);
}

MACRO_TEST(map_min_shapes_erase) {
    macro_map_t *root=NULL;
    Node a={.k=1}, b={.k=0}, c={.k=2};
    // single
    MACRO_ASSERT_TRUE(map_insert(&root,&a));
    assert_root_invariants(root);
    MACRO_ASSERT_TRUE(macro_map_erase(&root, &a.link));
    MACRO_ASSERT_TRUE(root == NULL);

    // two (left child)
    root=NULL;
    MACRO_ASSERT_TRUE(map_insert(&root,&a));
    MACRO_ASSERT_TRUE(map_insert(&root,&b));
    assert_root_invariants(root);
    MACRO_ASSERT_TRUE(macro_map_erase(&root, &a.link));
    MACRO_ASSERT_TRUE(root == &b.link);
    assert_root_invariants(root);
    MACRO_ASSERT_TRUE(macro_map_erase(&root, &b.link));
    MACRO_ASSERT_TRUE(root == NULL);

    // three (both sides)
    root=NULL;
    MACRO_ASSERT_TRUE(map_insert(&root,&a));
    MACRO_ASSERT_TRUE(map_insert(&root,&b));
    MACRO_ASSERT_TRUE(map_insert(&root,&c));
    check_invariants(root);
    MACRO_ASSERT_TRUE(macro_map_erase(&root, &a.link)); // root with two children
    check_invariants(root);
    while (root) {
        macro_map_t *p = macro_map_first(root);
        Node *n = macro_parent_object(p, Node, link);
        MACRO_ASSERT_TRUE(macro_map_erase(&root, &n->link));
        check_invariants(root);
    }
}


MACRO_TEST(map_erase_all_random_perms) {
    enum { N=128 };
    Node *nodes = calloc(N, sizeof(*nodes));
    for (int r=0;r<3;++r) {
        macro_map_t *root=NULL;
        int order[N];
        for (int i=0;i<N;++i) {
            nodes[i].k = i; nodes[i].v=i;
            MACRO_ASSERT_TRUE(map_insert(&root, &nodes[i]));
            order[i]=i;
        }
        shuffle(order, N);
        for (int i=0;i<N;++i) {
            int k = order[i];
            Node *hit = map_find_kv(root, &k);
            MACRO_ASSERT_TRUE(hit!=NULL);
            MACRO_ASSERT_TRUE(macro_map_erase(&root, &hit->link));
            if ((i & 7)==0) check_invariants(root);
        }
        MACRO_ASSERT_TRUE(root==NULL);
    }
    free(nodes);
}


MACRO_TEST(mmap_duplicate_addrs_are_ordered) {
    macro_map_t *root=NULL;
    Node a={.k=5}, b={.k=5}, c={.k=5};
    Node *arr[3]={&a,&b,&c};
    // Shuffle to vary insertion address order
    int perm[3]={0,1,2}; shuffle(perm,3);
    for (int i=0;i<3;++i) MACRO_ASSERT_TRUE(mmap_insert(&root, arr[perm[i]]));

    // Inorder among key==5 must be strictly by pointer address
    macro_map_t *p = macro_map_first(root);
    while (p && macro_parent_object(p, Node, link)->k != 5) p = macro_map_next(p);
    MACRO_ASSERT_TRUE(p != NULL);
    Node *first = macro_parent_object(p, Node, link);
    p = macro_map_next(p);
    MACRO_ASSERT_TRUE(p != NULL && macro_parent_object(p, Node, link)->k == 5);
    Node *second = macro_parent_object(p, Node, link);
    p = macro_map_next(p);
    MACRO_ASSERT_TRUE(p != NULL && macro_parent_object(p, Node, link)->k == 5);
    Node *third = macro_parent_object(p, Node, link);

    // Check address order
    MACRO_ASSERT_TRUE(first < second && second < third);
}


MACRO_TEST(map_full_forward_backward_walk) {
    macro_map_t *root=NULL; Node n[50];
    for (int i=0;i<50;++i){ n[i].k=i; MACRO_ASSERT_TRUE(map_insert(&root,&n[i])); }
    check_invariants(root);
    size_t cnt=0;
    for (macro_map_t *p=macro_map_first(root); p; p=macro_map_next(p)) ++cnt;
    MACRO_ASSERT_EQ_INT((int)cnt, 50);

    // reverse walk
    cnt=0;
    for (macro_map_t *p=macro_map_last(root); p; p=macro_map_previous(p)) ++cnt;
    MACRO_ASSERT_EQ_INT((int)cnt, 50);
}


static void assert_structural_equivalence(macro_map_t *a, macro_map_t *b) {
    if (!a || !b) { MACRO_ASSERT_TRUE(a==b); return; }
    Node *na = macro_parent_object(a, Node, link);
    Node *nb = macro_parent_object(b, Node, link);
    MACRO_ASSERT_EQ_INT(na->k, nb->k);
    MACRO_ASSERT_TRUE(macro_map_color(a) == macro_map_color(b));
    assert_structural_equivalence(a->left,  b->left);
    assert_structural_equivalence(a->right, b->right);
}
MACRO_TEST(map_copy_structure_preserved) {
    macro_map_t *root=NULL; Node n[64];
    for (int i=0;i<64;++i){ n[i].k=(i*37)%251; MACRO_ASSERT_TRUE(map_insert(&root,&n[i])); }
    macro_map_t *cpy = macro_map_copy(root, copy_cb_impl, NULL);
    assert_structural_equivalence(root, cpy);
    // free copy tree
    for (macro_map_t *p = macro_map_postorder_first(cpy); p; ) {
        macro_map_t *nx = macro_map_postorder_next(p);
        free(macro_parent_object(p, Node, link));
        p=nx;
    }
}


MACRO_TEST(map_long_alternating_ops) {
    macro_map_t *root=NULL;
    enum { R=2000 };
    Node *pool = calloc(R, sizeof(*pool));
    bool live[R]={0};

    for (int step=0; step<R; ++step) {
        int k = rand()%1000;
        if (live[k]) {
            Node *hit = map_find_kv(root, &k);
            if (hit) { MACRO_ASSERT_TRUE(macro_map_erase(&root, &hit->link)); live[k]=false; }
        } else {
            pool[k].k = k;
            bool ok = map_insert(&root, &pool[k]);
            live[k] = ok || live[k]; // ok==false if duplicate
        }
        if ((step & 31)==0) check_invariants(root);
    }
    // Cleanup: erase all remaining
    for (int k=0;k<1000;++k) if (live[k]) {
        Node *hit = map_find_kv(root,&k);
        if (hit) MACRO_ASSERT_TRUE(macro_map_erase(&root, &hit->link));
    }
    MACRO_ASSERT_TRUE(root==NULL);
    free(pool);
}


typedef struct FNode {
    macro_map_t link;   /* first member */
    int k, v;
} FNode;

static int fnode_cmp_noarg(const FNode *a, const FNode *b) {
    return (a->k > b->k) - (a->k < b->k);
}
static int fkv_cmp_noarg(const int *key, const FNode *n) {
    return (*key > n->k) - (*key < n->k);
}
macro_map_insert          (fmap_insert,  FNode, fnode_cmp_noarg)           /* no with_field */
_macro_map_kv             (fmap_lower_kv, lower_bound, macro_map_default(), int, FNode, fkv_cmp_noarg)
_macro_map_kv             (fmap_upper_kv, upper_bound, macro_map_default(), int, FNode, fkv_cmp_noarg)
macro_map_find_kv         (fmap_find_kv, int, FNode, fkv_cmp_noarg)

MACRO_TEST(map_no_field_api_first_member) {
    macro_map_t *root = NULL;
    FNode a={.k=3,.v=30}, b={.k=1,.v=10}, c={.k=5,.v=50}, d={.k=3,.v=31};

    MACRO_ASSERT_TRUE(fmap_insert(&root, &a));
    MACRO_ASSERT_TRUE(fmap_insert(&root, &b));
    MACRO_ASSERT_TRUE(fmap_insert(&root, &c));
    MACRO_ASSERT_TRUE(!fmap_insert(&root, &d)); /* unique reject */
    int k=3;
    FNode *h = fmap_find_kv(root, &k);
    MACRO_ASSERT_TRUE(h && h->k==3 && h==&a);

    k=4; FNode *lb=fmap_lower_kv(root,&k), *ub=fmap_upper_kv(root,&k);
    MACRO_ASSERT_TRUE(lb && lb->k==5);
    MACRO_ASSERT_TRUE(ub && ub->k==5);
}

MACRO_TEST(map_iterator_stitch_after_erase) {
    macro_map_t *root=NULL;
    enum { N=50 };
    Node nodes[N];
    for (int i=0;i<N;++i){ nodes[i].k=i; nodes[i].v=i; MACRO_ASSERT_TRUE(map_insert(&root,&nodes[i])); }
    check_invariants(root);

    /* erase every 3rd node; neighbors must link around it */
    for (int i=1;i<N-1;i+=3) {
        Node *cur = map_find_kv(root, &i);
        MACRO_ASSERT_TRUE(cur);
        macro_map_t *l = macro_map_previous(&cur->link);
        macro_map_t *r = macro_map_next(&cur->link);
        MACRO_ASSERT_TRUE(macro_map_erase(&root, &cur->link));
        check_invariants(root);
        if (l && r) {
            Node *ln = macro_parent_object(l, Node, link);
            Node *rn = macro_parent_object(r, Node, link);
            MACRO_ASSERT_EQ_INT(ln->k + 1, rn->k - 1); /* loose sanity */
        }
    }
    assert_inorder_sorted(root);
}


MACRO_TEST(map_extreme_keys_bounds) {
    macro_map_t *root=NULL;
    Node a={.k=INT_MIN,.v=1}, b={.k=0,.v=2}, c={.k=INT_MAX,.v=3};
    MACRO_ASSERT_TRUE(map_insert(&root,&a));
    MACRO_ASSERT_TRUE(map_insert(&root,&b));
    MACRO_ASSERT_TRUE(map_insert(&root,&c));
    check_invariants(root);

    int k;
    k=INT_MIN; MACRO_ASSERT_TRUE(map_lower_bound_kv(root,&k)->k == INT_MIN);
    k=INT_MIN; MACRO_ASSERT_TRUE(map_floor_kv(root,&k)->k       == INT_MIN);
    k=INT_MIN; MACRO_ASSERT_TRUE(map_ceiling_kv(root,&k)->k     == INT_MIN);
    k=INT_MIN; MACRO_ASSERT_TRUE(map_upper_bound_kv(root,&k)->k == 0);

    k=INT_MAX; MACRO_ASSERT_TRUE(map_upper_bound_kv(root,&k) == NULL);
    k=INT_MAX; MACRO_ASSERT_TRUE(map_floor_kv(root,&k)->k    == INT_MAX);
    k=INT_MAX; MACRO_ASSERT_TRUE(map_ceiling_kv(root,&k)->k  == INT_MAX);
}

MACRO_TEST(map_copy_empty_and_singleton) {
    /* empty */
    macro_map_t *cpy = macro_map_copy(NULL, copy_cb_impl, NULL);
    MACRO_ASSERT_TRUE(cpy == NULL);

    /* singleton */
    macro_map_t *root=NULL;
    Node n={.k=7,.v=1};
    MACRO_ASSERT_TRUE(map_insert(&root,&n));
    macro_map_t *cpy2 = macro_map_copy(root, copy_cb_impl, NULL);
    check_invariants(root); check_invariants(cpy2);

    int k=7;
    Node *a=map_find_kv(root,&k), *b=map_find_kv(cpy2,&k);
    MACRO_ASSERT_TRUE(a && b && a!=b);

    MACRO_ASSERT_TRUE(macro_map_erase(&cpy2,&b->link));
    MACRO_ASSERT_TRUE(map_find_kv(cpy2,&k)==NULL);
    MACRO_ASSERT_TRUE(map_find_kv(root,&k)!=NULL);

    for (macro_map_t *p=macro_map_postorder_first(cpy2); p;) {
        macro_map_t *nxt=macro_map_postorder_next(p);
        Node *nv=macro_parent_object(p,Node,link); free(nv); p=nxt;
    }
}


macro_map_find_with_field(map_find_node, link, Node, node_cmp_noarg)

MACRO_TEST(map_kv_vs_nonkv_parity) {
    macro_map_t *root=NULL;
    Node x[]={ {.k=4},{.k=1},{.k=9},{.k=4} };
    MACRO_ASSERT_TRUE(map_insert(&root,&x[0]));
    MACRO_ASSERT_TRUE(map_insert(&root,&x[1]));
    MACRO_ASSERT_TRUE(map_insert(&root,&x[2]));
    MACRO_ASSERT_TRUE(!map_insert(&root,&x[3])); /* unique */

    int k=4;
    Node *a = map_find_kv(root,&k);
    Node key4 = {.k=4};
    Node *b = map_find_node(root,&key4);
    MACRO_ASSERT_TRUE(a==b && a && a->k==4);
}





/* ----- Runner ----- */

int main(void) {
    macro_test_case tests[256];
    size_t test_count = 0;
    printf("[seed] unique=%d, mmap=%d, fuzz1=%d, fuzz2=%d\n", 12345, 4242, 777, 4242);


    MACRO_ADD(tests, map_min_shapes_erase);
    MACRO_ADD(tests, map_erase_all_random_perms);
    MACRO_ADD(tests, mmap_duplicate_addrs_are_ordered);
    MACRO_ADD(tests, map_full_forward_backward_walk);
    MACRO_ADD(tests, map_copy_structure_preserved);
    MACRO_ADD(tests, map_long_alternating_ops);

    MACRO_ADD(tests, map_unique_insert_find_iter);
    MACRO_ADD(tests, map_bounds_and_edges);
    MACRO_ADD(tests, map_erase_root_and_shapes);
    MACRO_ADD(tests, map_erase_leaf_one_two_children);
    MACRO_ADD(tests, multimap_duplicates_and_bounds);
    MACRO_ADD(tests, map_copy_and_independence);

    /* new iterator / bounds / erase / rotate / packing tests */
    MACRO_ADD(tests, map_iterator_edges);
    MACRO_ADD(tests, map_bounds_full_matrix);
    MACRO_ADD(tests, map_rotate_guard_root_null);
    MACRO_ADD(tests, map_parent_color_packing);

    /* fuzz and stress */
    MACRO_ADD(tests, map_random_fuzz_unique_vs_vector);
    MACRO_ADD(tests, mmap_random_fuzz_with_dups);
    MACRO_ADD(tests, map_copy_large);

    MACRO_ADD(tests, map_no_field_api_first_member);
    MACRO_ADD(tests, map_iterator_stitch_after_erase);
    MACRO_ADD(tests, map_extreme_keys_bounds);
    MACRO_ADD(tests, map_copy_empty_and_singleton);
    MACRO_ADD(tests, map_kv_vs_nonkv_parity);

    macro_run_all("the-macro-library/macro_map", tests, test_count);
    return 0;
}
