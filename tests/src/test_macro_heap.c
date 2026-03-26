// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_heap.h"
#include "the-macro-library/macro_test.h"

/* ---------- helpers / comparators ---------- */
static bool int_less(const int *a, const int *b) { return *a < *b; }

static bool is_min_heap_int(const int *a, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        size_t l = 2*i + 1, r = 2*i + 2;
        if (l < n && a[l] < a[i]) return false;
        if (r < n && a[r] < a[i]) return false;
    }
    return true;
}
static bool is_max_heap_int(const int *a, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        size_t l = 2*i + 1, r = 2*i + 2;
        if (l < n && a[l] > a[i]) return false;
        if (r < n && a[r] > a[i]) return false;
    }
    return true;
}

/* ---------- heap types + API generation ---------- */
macro_heap_t(IntHeap, int);

macro_min_heap_push          (min_heap_push_int,  less_no_arg, int, int_less, IntHeap)
macro_min_heap_pop           (min_heap_pop_int,   less_no_arg, int, int_less, IntHeap)
macro_min_heap_replace       (min_heap_replace_int, less_no_arg, int, int_less, IntHeap)

macro_max_heap_push          (max_heap_push_int,  less_no_arg, int, int_less, IntHeap)
macro_max_heap_pop           (max_heap_pop_int,   less_no_arg, int, int_less, IntHeap)
macro_max_heap_replace       (max_heap_replace_int, less_no_arg, int, int_less, IntHeap)

/* compare-API (runtime comparator) */
macro_min_heap_push_compare     (min_heap_push_int_cmp,  less_no_arg, int, IntHeap)
macro_min_heap_pop_compare      (min_heap_pop_int_cmp,   less_no_arg, int, IntHeap)
macro_min_heap_replace_compare  (min_heap_replace_int_cmp, less_no_arg, int, IntHeap)

macro_max_heap_push_compare     (max_heap_push_int_cmp,  less_no_arg, int, IntHeap)
macro_max_heap_pop_compare      (max_heap_pop_int_cmp,   less_no_arg, int, IntHeap)
macro_max_heap_replace_compare  (max_heap_replace_int_cmp, less_no_arg, int, IntHeap)

/* helpers to use API correctly: put value at arr[num], then push */
static void push_min(IntHeap *h, int x) { h->arr[h->num] = x; min_heap_push_int(h); }
static void push_max(IntHeap *h, int x) { h->arr[h->num] = x; max_heap_push_int(h); }
static void push_min_cmp(IntHeap *h, int x) { h->arr[h->num] = x; min_heap_push_int_cmp(h, int_less); }
static void push_max_cmp(IntHeap *h, int x) { h->arr[h->num] = x; max_heap_push_int_cmp(h, int_less); }

/* ---------- tests ---------- */

MACRO_TEST(heap_min_push_pop_basic) {
    int buf[128];
    IntHeap h = { .arr = buf, .num = 0, .size = 128 };

    int vals[] = {5, 1, 7, 7, -3, 0, 9, 2, 2, 2};
    size_t N = sizeof(vals)/sizeof(vals[0]);

    for (size_t i = 0; i < N; ++i) {
        push_min(&h, vals[i]);
        MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));
    }

    /* pop should be non-decreasing */
    int last = -1000000;
    for (size_t i = 0; i < N; ++i) {
        int *p = min_heap_pop_int(&h);
        MACRO_ASSERT_TRUE(p != NULL);
        MACRO_ASSERT_TRUE(*p >= last);
        last = *p;
        MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));
    }
    MACRO_ASSERT_TRUE(min_heap_pop_int(&h) == NULL);
}

MACRO_TEST(heap_max_push_pop_basic) {
    int buf[128];
    IntHeap h = { .arr = buf, .num = 0, .size = 128 };

    int vals[] = {5, 1, 7, 7, -3, 0, 9, 2, 2, 2};
    size_t N = sizeof(vals)/sizeof(vals[0]);

    for (size_t i = 0; i < N; ++i) {
        push_max(&h, vals[i]);
        MACRO_ASSERT_TRUE(is_max_heap_int(h.arr, h.num));
    }

    /* pop should be non-increasing */
    int last = 1000000;
    for (size_t i = 0; i < N; ++i) {
        int *p = max_heap_pop_int(&h);
        MACRO_ASSERT_TRUE(p != NULL);
        MACRO_ASSERT_TRUE(*p <= last);
        last = *p;
        MACRO_ASSERT_TRUE(is_max_heap_int(h.arr, h.num));
    }
    MACRO_ASSERT_TRUE(max_heap_pop_int(&h) == NULL);
}

/* This exposes the 2-element replace bug in macro_check_min_heap_zero */
MACRO_TEST(heap_min_replace_two_elements) {
    int buf[2];
    IntHeap h = { .arr = buf, .num = 0, .size = 2 };
    push_min(&h, 1);
    push_min(&h, 9);
    MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));

    /* Replace root with bigger value; expect sift-down to swap */
    h.arr[0] = 10;
    min_heap_replace_int(&h);
    MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));
    MACRO_ASSERT_EQ_INT(h.arr[0], 9);     /* expect {9,10} after fix */
}

/* This exposes “only left child checked” bug in macro_check_min_heap_zero */
MACRO_TEST(heap_min_replace_check_both_children) {
    int buf[3];
    IntHeap h = { .arr = buf, .num = 0, .size = 3 };
    push_min(&h, 1);
    push_min(&h, 5);
    push_min(&h, 2);
    MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));   /* heap is valid */

    /* Make root = 3: root<left(true) but root>right(false) -> must fix */
    h.arr[0] = 3;
    min_heap_replace_int(&h);
    MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));   /* fails before fix */
}

/* Compare-API parity with non-compare */
MACRO_TEST(heap_min_compare_matches) {
    int a[64], b[64];
    IntHeap h1 = { .arr = a, .num = 0, .size = 64 };
    IntHeap h2 = { .arr = b, .num = 0, .size = 64 };

    int vals[] = {4,1,6,3,5,2,2,2,-1,0,9};
    size_t N = sizeof(vals)/sizeof(vals[0]);
    for (size_t i = 0; i < N; ++i) { push_min(&h1, vals[i]); push_min_cmp(&h2, vals[i]); }

    for (size_t i = 0; i < N; ++i) {
        int *p1 = min_heap_pop_int(&h1);
        int *p2 = min_heap_pop_int_cmp(&h2, int_less);
        MACRO_ASSERT_TRUE(p1 && p2);
        MACRO_ASSERT_EQ_INT(*p1, *p2);
    }
}

MACRO_TEST(heap_max_compare_matches) {
    int a[64], b[64];
    IntHeap h1 = { .arr = a, .num = 0, .size = 64 };
    IntHeap h2 = { .arr = b, .num = 0, .size = 64 };

    int vals[] = {4,1,6,3,5,2,2,2,-1,0,9};
    size_t N = sizeof(vals)/sizeof(vals[0]);
    for (size_t i = 0; i < N; ++i) { push_max(&h1, vals[i]); push_max_cmp(&h2, vals[i]); }

    for (size_t i = 0; i < N; ++i) {
        int *p1 = max_heap_pop_int(&h1);
        int *p2 = max_heap_pop_int_cmp(&h2, int_less);
        MACRO_ASSERT_TRUE(p1 && p2);
        MACRO_ASSERT_EQ_INT(*p1, *p2);
    }
}

/* Early-return path for replace: nothing to do if already valid */
MACRO_TEST(heap_min_replace_already_valid) {
    int buf[3];
    IntHeap h = { .arr = buf, .num = 0, .size = 3 };
    push_min(&h, 1);
    push_min(&h, 5);
    push_min(&h, 7);

    h.arr[0] = 0;                  /* now root is even smaller: still valid */
    min_heap_replace_int(&h);      /* should early-return */
    MACRO_ASSERT_TRUE(is_min_heap_int(h.arr, h.num));
    MACRO_ASSERT_EQ_INT(h.arr[0], 0);
}

/* ---------- runner ---------- */
int main(void) {
    macro_test_case tests[256];
    size_t test_count = 0;

    MACRO_ADD(tests, heap_min_push_pop_basic);
    MACRO_ADD(tests, heap_max_push_pop_basic);

    /* The next two fail with current macro_check_*_heap_zero; they pass after the fix above */
    MACRO_ADD(tests, heap_min_replace_two_elements);
    MACRO_ADD(tests, heap_min_replace_check_both_children);

    MACRO_ADD(tests, heap_min_compare_matches);
    MACRO_ADD(tests, heap_max_compare_matches);
    MACRO_ADD(tests, heap_min_replace_already_valid);

    macro_run_all("the-macro-library/macro_heap", tests, test_count);
    return 0;
}
