// SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_bsearch.h"
#include "the-macro-library/macro_test.h"

/* ---------- Types & comparators ---------- */
static int int_cmp_noarg(const int *a, const int *b) {
    return (*a > *b) - (*a < *b);
}

typedef struct { int k, v; } pair_t;
static int kv_cmp_noarg(const int *key, const pair_t *p) {
    return (*key > p->k) - (*key < p->k);
}

macro_bsearch_compare_h           (bs_core_int_cmp,           int);
macro_bsearch_kv_compare_h        (bs_core_kv_cmp,            int, pair_t);

/* ---------- Generate bsearch functions (non-compare variants) ---------- */
/* Value-type (int) using compile-time comparator symbol */
macro_bsearch         (bs_core_int,           int, int_cmp_noarg)
macro_bsearch_first   (bs_first_int,          int, int_cmp_noarg)
macro_bsearch_last    (bs_last_int,           int, int_cmp_noarg)
macro_bsearch_lower_bound (bs_lower_bound_int,    int, int_cmp_noarg)
macro_bsearch_upper_bound (bs_upper_bound_int,    int, int_cmp_noarg)
macro_bsearch_floor       (bs_floor_int,          int, int_cmp_noarg)
macro_bsearch_ceiling     (bs_ceiling_int,        int, int_cmp_noarg)

/* KV (key=int, value=pair_t) */
macro_bsearch_kv           (bs_core_kv,           int, pair_t, kv_cmp_noarg)
macro_bsearch_first_kv     (bs_first_kv,          int, pair_t, kv_cmp_noarg)
macro_bsearch_last_kv      (bs_last_kv,           int, pair_t, kv_cmp_noarg)
macro_bsearch_lower_bound_kv (bs_lower_bound_kv,  int, pair_t, kv_cmp_noarg)
macro_bsearch_upper_bound_kv (bs_upper_bound_kv,  int, pair_t, kv_cmp_noarg)
macro_bsearch_floor_kv       (bs_floor_kv,        int, pair_t, kv_cmp_noarg)
macro_bsearch_ceiling_kv     (bs_ceiling_kv,      int, pair_t, kv_cmp_noarg)

/* Compare-API declarations/definitions that rely on compare_ ## style */
macro_bsearch_compare           (bs_core_int_cmp,           int)
macro_bsearch_first_compare       (bs_first_int_cmp,          int)
macro_bsearch_last_compare        (bs_last_int_cmp,           int)
macro_bsearch_lower_bound_compare (bs_lower_bound_int_cmp,    int)
macro_bsearch_upper_bound_compare (bs_upper_bound_int_cmp,    int)
macro_bsearch_floor_compare       (bs_floor_int_cmp,          int)
macro_bsearch_ceiling_compare     (bs_ceiling_int_cmp,        int)

macro_bsearch_kv_compare        (bs_core_kv_cmp,            int, pair_t)
macro_bsearch_first_kv_compare    (bs_first_kv_cmp,           int, pair_t)
macro_bsearch_last_kv_compare     (bs_last_kv_cmp,            int, pair_t)
macro_bsearch_lower_bound_kv_compare (bs_lower_bound_kv_cmp,  int, pair_t)
macro_bsearch_upper_bound_kv_compare (bs_upper_bound_kv_cmp,  int, pair_t)
macro_bsearch_floor_kv_compare       (bs_floor_kv_cmp,        int, pair_t)
macro_bsearch_ceiling_kv_compare     (bs_ceiling_kv_cmp,      int, pair_t)

/* ---------- Fixtures ---------- */
static int A[] = {1,2,2,2,4,5};
static const size_t AN = sizeof(A)/sizeof(A[0]);

static pair_t Pairs[] = {
    {1,10},{2,20},{2,21},{2,22},{4,40},{5,50}
};
static const size_t PN = sizeof(Pairs)/sizeof(Pairs[0]);

/* ---------- Tests: value-type (int) ---------- */

MACRO_TEST(bsearch_value_core_and_bounds) {
    int key2 = 2, key3 = 3, key6 = 6, key0 = 0;

    int *p2 = bs_core_int(&key2, A, AN);
    MACRO_ASSERT_TRUE(p2 && *p2 == 2);

    /* first/last of duplicates */
    int *pf = bs_first_int(&key2, A, AN);
    int *pl = bs_last_int (&key2, A, AN);
    MACRO_ASSERT_TRUE(pf == &A[1]);
    MACRO_ASSERT_TRUE(pl == &A[3]);

    /* lower/upper bound */
    MACRO_ASSERT_TRUE(bs_lower_bound_int(&key3, A, AN) == &A[4]); /* first >=3 => 4 */
    MACRO_ASSERT_TRUE(bs_upper_bound_int(&key2, A, AN) == &A[4]); /* first >2  => 4 */

    /* floor/ceiling */
    MACRO_ASSERT_TRUE(bs_floor_int(&key3, A, AN)   == &A[3]);     /* last <=3  => 2 at idx 3 */
    MACRO_ASSERT_TRUE(bs_ceiling_int(&key3, A, AN) == &A[4]);     /* first >=3 => 4 */

    /* edges: not found */
    MACRO_ASSERT_TRUE(bs_core_int(&key6, A, AN) == NULL);
    MACRO_ASSERT_TRUE(bs_lower_bound_int(&key6, A, AN) == NULL);  /* beyond end */
    MACRO_ASSERT_TRUE(bs_upper_bound_int(&key6, A, AN) == NULL);
    MACRO_ASSERT_TRUE(bs_floor_int(&key0, A, AN) == NULL);        /* before begin */
    MACRO_ASSERT_TRUE(bs_ceiling_int(&key6, A, AN) == NULL);
}

/* Empty / singleton arrays */
MACRO_TEST(bsearch_value_empty_and_singleton) {
    int key = 1;
    int empty[1] = {0};
    MACRO_ASSERT_TRUE(bs_core_int(&key, empty, 0) == NULL);
    MACRO_ASSERT_TRUE(bs_first_int(&key, empty, 0) == NULL);
    MACRO_ASSERT_TRUE(bs_last_int(&key, empty, 0) == NULL);
    MACRO_ASSERT_TRUE(bs_lower_bound_int(&key, empty, 0) == NULL);
    MACRO_ASSERT_TRUE(bs_upper_bound_int(&key, empty, 0) == NULL);
    MACRO_ASSERT_TRUE(bs_floor_int(&key, empty, 0) == NULL);
    MACRO_ASSERT_TRUE(bs_ceiling_int(&key, empty, 0) == NULL);

    int one[] = {3};
    MACRO_ASSERT_TRUE(bs_core_int(&key, one, 1) == NULL);
    int *lb = bs_lower_bound_int(&key, one, 1);
    MACRO_ASSERT_TRUE(lb == &one[0]); /* first >=1 is 3 */
    MACRO_ASSERT_TRUE(bs_floor_int(&key, one, 1) == NULL); /* no <=1 */
}

/* ---------- Tests: KV (key=int, value=pair_t) ---------- */

MACRO_TEST(bsearch_kv_core_first_last) {
    int k2 = 2, k3 = 3, k6 = 6, k0 = 0;

    pair_t *any = bs_core_kv(&k2, Pairs, PN);
    MACRO_ASSERT_TRUE(any && any->k == 2);

    pair_t *pf = bs_first_kv(&k2, Pairs, PN);
    pair_t *pl = bs_last_kv (&k2, Pairs, PN);
    MACRO_ASSERT_TRUE(pf == &Pairs[1]);
    MACRO_ASSERT_TRUE(pl == &Pairs[3]);

    MACRO_ASSERT_TRUE(bs_lower_bound_kv(&k3, Pairs, PN) == &Pairs[4]); /* first k>=3 */
    MACRO_ASSERT_TRUE(bs_upper_bound_kv(&k2, Pairs, PN) == &Pairs[4]); /* first k>2  */

    MACRO_ASSERT_TRUE(bs_floor_kv(&k3, Pairs, PN)   == &Pairs[3]);     /* last k<=3  */
    MACRO_ASSERT_TRUE(bs_ceiling_kv(&k3, Pairs, PN) == &Pairs[4]);     /* first k>=3 */

    MACRO_ASSERT_TRUE(bs_core_kv(&k6, Pairs, PN) == NULL);
    MACRO_ASSERT_TRUE(bs_lower_bound_kv(&k6, Pairs, PN) == NULL);
    MACRO_ASSERT_TRUE(bs_upper_bound_kv(&k6, Pairs, PN) == NULL);
    MACRO_ASSERT_TRUE(bs_floor_kv(&k0, Pairs, PN) == NULL);
    MACRO_ASSERT_TRUE(bs_ceiling_kv(&k6, Pairs, PN) == NULL);
}

MACRO_TEST(bsearch_value_ceiling_equals_lower_bound) {
    int A[] = {1,2,2,2,4,5};
    size_t AN = sizeof(A)/sizeof(A[0]);
    for (int k = 0; k <= 6; ++k) {
        int *lb = bs_lower_bound_int(&k, A, AN);
        int *ce = bs_ceiling_int(&k, A, AN);
        MACRO_ASSERT_TRUE(lb == ce);
    }
}

MACRO_TEST(bsearch_value_ceiling_edges) {
    int A[] = {1,2,2,2,4,5};
    size_t AN = sizeof(A)/sizeof(A[0]);

    int k2 = 2;  MACRO_ASSERT_EQ_INT((int)(bs_ceiling_int(&k2, A, AN) - A), 1);
    int k0 = 0;  MACRO_ASSERT_EQ_INT((int)(bs_ceiling_int(&k0, A, AN) - A), 0);
    int k3 = 3;  MACRO_ASSERT_EQ_INT((int)(bs_ceiling_int(&k3, A, AN) - A), 4);
    int k6 = 6;  MACRO_ASSERT_TRUE(bs_ceiling_int(&k6, A, AN) == NULL);
}

MACRO_TEST(bsearch_value_floor_edges) {
    int A[] = {1,2,2,2,4,5};
    size_t AN = sizeof(A)/sizeof(A[0]);

    int k2 = 2;  MACRO_ASSERT_EQ_INT((int)(bs_floor_int(&k2, A, AN) - A), 3);
    int k0 = 0;  MACRO_ASSERT_TRUE(bs_floor_int(&k0, A, AN) == NULL);
    int k3 = 3;  MACRO_ASSERT_EQ_INT((int)(bs_floor_int(&k3, A, AN) - A), 3);
    int k6 = 6;  MACRO_ASSERT_EQ_INT((int)(bs_floor_int(&k6, A, AN) - A), 5);
}

MACRO_TEST(bsearch_bounds_edges) {
    int A[] = {1, 2, 4};
    size_t AN = sizeof(A)/sizeof(A[0]);

    int k0 = 0; MACRO_ASSERT_EQ_INT(bs_lower_bound_int(&k0, A, AN) - A, 0);
    int k3 = 3; MACRO_ASSERT_EQ_INT(bs_lower_bound_int(&k3, A, AN) - A, 2);
    int k5 = 5; MACRO_ASSERT_TRUE(bs_lower_bound_int(&k5, A, AN) == NULL);

    int k2 = 2; MACRO_ASSERT_EQ_INT(bs_upper_bound_int(&k2, A, AN) - A, 2);
    int k4 = 4; MACRO_ASSERT_TRUE(bs_upper_bound_int(&k4, A, AN) == NULL);
}

MACRO_TEST(bsearch_value_compare_variants_match) {
    int key2=2, key3=3, key6=6;

    /* core/first/last */
    int *p  = bs_core_int_cmp(&key2, A, AN, int_cmp_noarg);
    int *pf = bs_first_int_cmp(&key2, A, AN, int_cmp_noarg);
    int *pl = bs_last_int_cmp (&key2, A, AN, int_cmp_noarg);
    MACRO_ASSERT_TRUE(p && *p == 2);
    MACRO_ASSERT_TRUE(pf == &A[1]);
    MACRO_ASSERT_TRUE(pl == &A[3]);

    /* bounds */
    MACRO_ASSERT_TRUE(bs_lower_bound_int_cmp(&key3, A, AN, int_cmp_noarg) == &A[4]);
    MACRO_ASSERT_TRUE(bs_upper_bound_int_cmp(&key2, A, AN, int_cmp_noarg) == &A[4]);

    /* floor/ceiling */
    MACRO_ASSERT_TRUE(bs_floor_int_cmp  (&key3, A, AN, int_cmp_noarg) == &A[3]);
    MACRO_ASSERT_TRUE(bs_ceiling_int_cmp(&key3, A, AN, int_cmp_noarg) == &A[4]);

    /* edges */
    MACRO_ASSERT_TRUE(bs_core_int_cmp(&key6, A, AN, int_cmp_noarg) == NULL);
    MACRO_ASSERT_TRUE(bs_lower_bound_int_cmp(&key6, A, AN, int_cmp_noarg) == NULL);
    MACRO_ASSERT_TRUE(bs_upper_bound_int_cmp(&key6, A, AN, int_cmp_noarg) == NULL);
    MACRO_ASSERT_TRUE(bs_ceiling_int_cmp(&key6, A, AN, int_cmp_noarg) == NULL);
}

MACRO_TEST(bsearch_kv_compare_variants_match) {
    int k2=2, k3=3, k6=6;

    pair_t *any = bs_core_kv_cmp(&k2, Pairs, PN, kv_cmp_noarg);
    MACRO_ASSERT_TRUE(any && any->k == 2);

    pair_t *pf = bs_first_kv_cmp(&k2, Pairs, PN, kv_cmp_noarg);
    pair_t *pl = bs_last_kv_cmp (&k2, Pairs, PN, kv_cmp_noarg);
    MACRO_ASSERT_TRUE(pf == &Pairs[1]);
    MACRO_ASSERT_TRUE(pl == &Pairs[3]);

    MACRO_ASSERT_TRUE(bs_lower_bound_kv_cmp(&k3, Pairs, PN, kv_cmp_noarg) == &Pairs[4]);
    MACRO_ASSERT_TRUE(bs_upper_bound_kv_cmp(&k2, Pairs, PN, kv_cmp_noarg) == &Pairs[4]);

    MACRO_ASSERT_TRUE(bs_floor_kv_cmp  (&k3, Pairs, PN, kv_cmp_noarg) == &Pairs[3]);
    MACRO_ASSERT_TRUE(bs_ceiling_kv_cmp(&k3, Pairs, PN, kv_cmp_noarg) == &Pairs[4]);

    MACRO_ASSERT_TRUE(bs_core_kv_cmp(&k6, Pairs, PN, kv_cmp_noarg) == NULL);
    MACRO_ASSERT_TRUE(bs_lower_bound_kv_cmp(&k6, Pairs, PN, kv_cmp_noarg) == NULL);
    MACRO_ASSERT_TRUE(bs_upper_bound_kv_cmp(&k6, Pairs, PN, kv_cmp_noarg) == NULL);
    MACRO_ASSERT_TRUE(bs_ceiling_kv_cmp(&k6, Pairs, PN, kv_cmp_noarg) == NULL);
}


/* ---------- Runner ---------- */

int main(void) {
    macro_test_case tests[256];
    size_t test_count = 0;

    MACRO_ADD(tests, bsearch_value_core_and_bounds);
    MACRO_ADD(tests, bsearch_value_empty_and_singleton);

    MACRO_ADD(tests, bsearch_kv_core_first_last);
    MACRO_ADD(tests, bsearch_value_ceiling_equals_lower_bound);
    MACRO_ADD(tests, bsearch_value_ceiling_edges);
    MACRO_ADD(tests, bsearch_value_floor_edges);
    MACRO_ADD(tests, bsearch_bounds_edges);
    MACRO_ADD(tests, bsearch_value_compare_variants_match);
    MACRO_ADD(tests, bsearch_kv_compare_variants_match);

    macro_run_all("the-macro-library/macro_bsearch", tests, test_count);
    return 0;
}
