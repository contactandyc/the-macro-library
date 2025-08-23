// SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "the-macro-library/macro_sort.h"
#include "the-macro-library/macro_test.h"

/* ---------- Types & comparators ---------- */
static bool int_less(const int *a, const int *b) { return *a < *b; }

typedef struct { int k, v; } pair_t;
static bool pair_less_kv(const pair_t *a, const pair_t *b) {
    if (a->k != b->k) return a->k < b->k;
    return a->v < b->v;
}

/* ---------- Generate sort functions ---------- */
/* value-type (int) */
macro_sort         (sort_int,     int,     int_less)
macro_sort_compare (sort_int_cmp, int)
/* KV pair */
macro_sort         (sort_pair,     pair_t,  pair_less_kv)
macro_sort_compare (sort_pair_cmp, pair_t)

/* ---------- Helpers ---------- */
static bool is_sorted_int(const int *a, size_t n) {
    for (size_t i = 1; i < n; ++i) if (a[i-1] > a[i]) return false;
    return true;
}
static bool is_sorted_pair(const pair_t *a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        if (a[i-1].k > a[i].k) return false;
        if (a[i-1].k == a[i].k && a[i-1].v > a[i].v) return false;
    }
    return true;
}
static void shuffle_int(int *a, size_t n, unsigned seed) {
    srand(seed);
    for (size_t i = n; i > 1; --i) {
        size_t j = (size_t)(rand() % i);
        int t = a[i-1]; a[i-1] = a[j]; a[j] = t;
    }
}
static void shuffle_pair(pair_t *a, size_t n, unsigned seed) {
    srand(seed);
    for (size_t i = n; i > 1; --i) {
        size_t j = (size_t)(rand() % i);
        pair_t t = a[i-1]; a[i-1] = a[j]; a[j] = t;
    }
}

/* ---------- Tests: ints ---------- */

MACRO_TEST(sort_int_empty_singleton) {
    int empty[1] = {0};
    sort_int(empty, 0);
    MACRO_ASSERT_TRUE(1); /* just don't crash */

    int one[] = {42};
    sort_int(one, 1);
    MACRO_ASSERT_TRUE(one[0] == 42);
}

MACRO_TEST(sort_int_two_elements) {
    int a1[] = {1,2};
    sort_int(a1, 2);
    MACRO_ASSERT_TRUE(is_sorted_int(a1, 2));

    int a2[] = {2,1};
    sort_int(a2, 2);
    MACRO_ASSERT_TRUE(is_sorted_int(a2, 2));
}

MACRO_TEST(sort_int_sorted_reverse_duplicates_negatives) {
    /* already sorted */
    int s1[] = {-5, -1, 0, 0, 3, 7, 9};
    sort_int(s1, sizeof s1/sizeof s1[0]);
    MACRO_ASSERT_TRUE(is_sorted_int(s1, sizeof s1/sizeof s1[0]));

    /* reverse sorted (exercises reverse fast-paths) */
    int s2[] = {9,7,3,0,0,-1,-5};
    sort_int(s2, sizeof s2/sizeof s2[0]);
    MACRO_ASSERT_TRUE(is_sorted_int(s2, sizeof s2/sizeof s2[0]));

    /* many duplicates */
    int s3[] = {5,5,5,5,5,5,5,5,5};
    sort_int(s3, sizeof s3/sizeof s3[0]);
    MACRO_ASSERT_TRUE(is_sorted_int(s3, sizeof s3/sizeof s3[0]));
}

MACRO_TEST(sort_int_threshold_sizes) {
    /* Around insertion-sort cutoff (<17), ninther (>40), and larger blocks */
    const int sizes[] = {0,1,2,7,8,16,17,40,41,64,255,256,257};
    for (size_t si = 0; si < sizeof(sizes)/sizeof(sizes[0]); ++si) {
        size_t n = (size_t)sizes[si];
        int *a = (int*)malloc(n * sizeof(int));
        int *b = (int*)malloc(n * sizeof(int));
        /* fill with a deterministic pattern including negatives and repeats */
        for (size_t i = 0; i < n; ++i) a[i] = (int)((37*i - 91) % 23); /* small domain -> duplicates */
        memcpy(b, a, n*sizeof(int));

        /* scramble then sort both variants */
        shuffle_int(a, n, 1234);
        shuffle_int(b, n, 1234);
        sort_int(a, n);
        sort_int_cmp(b, n, int_less);

        MACRO_ASSERT_TRUE(is_sorted_int(a, n));
        MACRO_ASSERT_TRUE(is_sorted_int(b, n));
        /* results should match element-by-element */
        for (size_t i = 0; i < n; ++i) MACRO_ASSERT_EQ_INT(a[i], b[i]);

        free(a); free(b);
    }
}

/* ---------- Tests: pairs (k,v) ---------- */

MACRO_TEST(sort_pair_basic_and_duplicates) {
    pair_t p[] = {
        {2,9},{1,5},{2,7},{1,3},{1,3},{3,0},{2,7},{-1,4}
    };
    pair_t q[sizeof p/sizeof p[0]];
    memcpy(q, p, sizeof p);

    sort_pair(p, sizeof p/sizeof p[0]);
    MACRO_ASSERT_TRUE(is_sorted_pair(p, sizeof p/sizeof p[0]));

    sort_pair_cmp(q, sizeof q/sizeof q[0], pair_less_kv);
    MACRO_ASSERT_TRUE(is_sorted_pair(q, sizeof q/sizeof q[0]));

    /* identical outputs (total order key->value) */
    for (size_t i = 0; i < sizeof p/sizeof p[0]; ++i) {
        MACRO_ASSERT_EQ_INT(p[i].k, q[i].k);
        MACRO_ASSERT_EQ_INT(p[i].v, q[i].v);
    }
}

MACRO_TEST(sort_pair_random_sizes) {
    const size_t N = 200;
    pair_t *p = (pair_t*)malloc(N*sizeof(pair_t));
    pair_t *q = (pair_t*)malloc(N*sizeof(pair_t));

    /* fill with repeated keys, varied values */
    for (size_t i = 0; i < N; ++i) {
        p[i].k = (int)((i*17 - 13) % 11) - 5;
        p[i].v = (int)((i*31 + 7) % 29) - 10;
    }
    memcpy(q, p, N*sizeof(pair_t));
    shuffle_pair(p, N, 2025);
    shuffle_pair(q, N, 2025);

    sort_pair(p, N);
    sort_pair_cmp(q, N, pair_less_kv);

    MACRO_ASSERT_TRUE(is_sorted_pair(p, N));
    MACRO_ASSERT_TRUE(is_sorted_pair(q, N));
    for (size_t i = 0; i < N; ++i) {
        MACRO_ASSERT_EQ_INT(p[i].k, q[i].k);
        MACRO_ASSERT_EQ_INT(p[i].v, q[i].v);
    }

    free(p); free(q);
}

/* ---------- Runner ---------- */

int main(void) {
    macro_test_case tests[256];
    size_t test_count = 0;

    MACRO_ADD(tests, sort_int_empty_singleton);
    MACRO_ADD(tests, sort_int_two_elements);
    MACRO_ADD(tests, sort_int_sorted_reverse_duplicates_negatives);
    MACRO_ADD(tests, sort_int_threshold_sizes);

    MACRO_ADD(tests, sort_pair_basic_and_duplicates);
    MACRO_ADD(tests, sort_pair_random_sizes);

    macro_run_all("the-macro-library/macro_sort", tests, test_count);
    return 0;
}
