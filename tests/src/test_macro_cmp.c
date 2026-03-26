// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_test.h"

/* ---------- Sample types & comparators ---------- */
static int int_cmp_noarg(const int *a, const int *b) {
    return (*a > *b) - (*a < *b);
}
static bool int_less_noarg(const int *a, const int *b) {
    return *a < *b;
}

/* arg-first: scale factor is in arg; positive normal, negative reverses */
static int int_arg_cmp(void *arg, const int *a, const int *b) {
    int scale = arg ? *(int *)arg : 1;
    int r = (*a > *b) - (*a < *b);
    return r * (scale >= 0 ? 1 : -1);
}

/* cmp-arg: same, but arg last */
static int int_cmp_arg(const int *a, const int *b, void *arg) {
    int scale = arg ? *(int *)arg : 1;
    int r = (*a > *b) - (*a < *b);
    return r * (scale >= 0 ? 1 : -1);
}

/* less with arg */
static bool int_arg_less(void *arg, const int *a, const int *b) {
    int scale = arg ? *(int *)arg : 1;
    return scale >= 0 ? (*a < *b) : (*a > *b);
}
static bool int_less_arg(const int *a, const int *b, void *arg) {
    int scale = arg ? *(int *)arg : 1;
    return scale >= 0 ? (*a < *b) : (*a > *b);
}

/* Key/Value comparators for *_kv path */
typedef struct { int k, v; } pair_t;
static int kv_cmp_noarg(const int *key, const pair_t *p) {
    return (*key > p->k) - (*key < p->k);
}
static int kv_arg_cmp(void *arg, const int *key, const pair_t *p) {
    int flip = arg ? *(int *)arg : 0;
    int r = (*key > p->k) - (*key < p->k);
    return flip ? -r : r;
}
static int kv_cmp_arg(const int *key, const pair_t *p, void *arg) {
    int flip = arg ? *(int *)arg : 0;
    int r = (*key > p->k) - (*key < p->k);
    return flip ? -r : r;
}

/* ---------- Tests for macro_less / macro_equal / macro_cmp ---------- */

MACRO_TEST(cmp_noarg_and_less_noarg) {
    int a = 3, b = 7, c = 3;

    /* Using explicit comparator (cmp_no_arg) */
    MACRO_ASSERT_TRUE( macro_less(cmp_no_arg,   int, int_cmp_noarg, &a, &b) );
    MACRO_ASSERT_TRUE(!macro_less(cmp_no_arg,   int, int_cmp_noarg, &b, &a) );
    MACRO_ASSERT_TRUE( macro_equal(cmp_no_arg,  int, int_cmp_noarg, &a, &c) );
    MACRO_ASSERT_TRUE(!macro_equal(cmp_no_arg,  int, int_cmp_noarg, &a, &b) );

    /* Using less predicate (less_no_arg) */
    MACRO_ASSERT_TRUE( macro_less(less_no_arg,  int, int_less_noarg, &a, &b) );
    MACRO_ASSERT_TRUE(!macro_less(less_no_arg,  int, int_less_noarg, &b, &a) );
    MACRO_ASSERT_TRUE( macro_equal(less_no_arg, int, int_less_noarg, &a, &c) );

    /* macro_cmp from less predicate should produce -1/0/1 style ordering */
    MACRO_ASSERT_EQ_INT(macro_cmp(less_no_arg,  int, int_less_noarg, &a, &b), -1);
    MACRO_ASSERT_EQ_INT(macro_cmp(less_no_arg,  int, int_less_noarg, &b, &a),  1);
    MACRO_ASSERT_EQ_INT(macro_cmp(less_no_arg,  int, int_less_noarg, &a, &c),  0);
}

MACRO_TEST(cmp_arg_first_and_last) {
    int a = 2, b = 5, same = 2;
    int pos = +1, neg = -1;

    /* arg_cmp (arg first) */
    #define arg (&pos)
    MACRO_ASSERT_TRUE( macro_less(arg_cmp, int, int_arg_cmp, &a, &b) );         /* normal */
    MACRO_ASSERT_TRUE(!macro_less(arg_cmp, int, int_arg_cmp, &b, &a) );
    #undef arg

    /* flip order with negative scale */
    #define arg (&neg)
    MACRO_ASSERT_TRUE( macro_less(arg_cmp, int, int_arg_cmp, &b, &a) );
    MACRO_ASSERT_TRUE( macro_equal(arg_cmp, int, int_arg_cmp, &a, &same) );
    #undef arg

    /* cmp_arg (arg last) */
    #define arg (&pos)
    MACRO_ASSERT_TRUE( macro_less(cmp_arg, int, int_cmp_arg, &a, &b) );
    #undef arg

    #define arg (&neg)
    MACRO_ASSERT_TRUE( macro_less(cmp_arg, int, int_cmp_arg, &b, &a) );
    #undef arg

    /* less_arg / arg_less predicate styles */
    #define arg (&pos)
    MACRO_ASSERT_TRUE( macro_less(less_arg, int, int_less_arg, &a, &b) );
    #undef arg

    #define arg (&neg)
    MACRO_ASSERT_TRUE( macro_less(arg_less, int, int_arg_less, &b, &a) );
    #undef arg
}

MACRO_TEST(equal_and_relations_helpers) {
    int a = 10, b = 20, c = 10;

    /* <=, >=, greater built from macro_cmp */
    MACRO_ASSERT_TRUE( macro_le(less_no_arg, int, int_less_noarg, &a, &b) );
    MACRO_ASSERT_TRUE( macro_ge(less_no_arg, int, int_less_noarg, &b, &a) );
    MACRO_ASSERT_TRUE( macro_ge(less_no_arg, int, int_less_noarg, &a, &c) );
    MACRO_ASSERT_TRUE( macro_greater(less_no_arg, int, int_less_noarg, &b, &a) );
    MACRO_ASSERT_TRUE(!macro_greater(less_no_arg, int, int_less_noarg, &a, &b) );
}

/* ---------- KV tests ---------- */

MACRO_TEST(cmp_kv_noarg_and_with_arg) {
    pair_t v[] = { {1,100},{2,200},{3,300} };
    int k2 = 2, k4 = 4;

    MACRO_ASSERT_TRUE(  macro_equal_kv(cmp_no_arg, int, pair_t, kv_cmp_noarg, &k2, &v[1]) );
    MACRO_ASSERT_TRUE(  macro_less_kv (cmp_no_arg, int, pair_t, kv_cmp_noarg, &k2, &v[2]) );
    MACRO_ASSERT_TRUE( !macro_equal_kv(cmp_no_arg, int, pair_t, kv_cmp_noarg, &k4, &v[2]) );

    int flip = 1;
    #define arg (&flip)
    /* with flip, ordering reverses; just smoke-test it routes arg */
    MACRO_ASSERT_TRUE( macro_greater_kv(arg_cmp, int, pair_t, kv_arg_cmp, &k2, &v[2]) );
    #undef arg

    #define arg (&flip)
    /* cmp_arg variant also sees arg */
    MACRO_ASSERT_TRUE( macro_cmp_kv(cmp_arg, int, pair_t, kv_cmp_arg, &k2, &v[1]) == 0 );
    #undef arg
}

/* ---------- Runner ---------- */

int main(void) {
    macro_test_case tests[128];
    size_t test_count = 0;

    MACRO_ADD(tests, cmp_noarg_and_less_noarg);
    MACRO_ADD(tests, cmp_arg_first_and_last);
    MACRO_ADD(tests, equal_and_relations_helpers);
    MACRO_ADD(tests, cmp_kv_noarg_and_with_arg);

    macro_run_all("the-macro-library/macro_cmp", tests, test_count);
    return 0;
}
