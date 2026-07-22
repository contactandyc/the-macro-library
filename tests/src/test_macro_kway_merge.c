// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#include <stdint.h>
#include <string.h>
#include "the-macro-library/macro_kway_merge.h"
#include "the-macro-library/macro_test.h"

typedef struct {
    int *curr;
    int *end;
} stream_iter_t;

macro_heap_t(IterHeap, stream_iter_t);

static bool iter_less(const stream_iter_t *a, const stream_iter_t *b) {
    return *(a->curr) < *(b->curr);
}

macro_min_heap_push(push_iter, less_no_arg, stream_iter_t, iter_less, IterHeap)
macro_min_heap_pop(pop_iter, less_no_arg, stream_iter_t, iter_less, IterHeap)

static int merged_output[100];
static int output_count = 0;

static void process_iter(stream_iter_t *it) {
    merged_output[output_count++] = *(it->curr);
}

static void advance_iter(stream_iter_t *it, IterHeap *h) {
    it->curr++;
    if (it->curr < it->end) {
        h->arr[h->num] = *it;
        push_iter(h);
    }
}

MACRO_TEST(kway_merge_basic_three_streams) {
    output_count = 0;
    int s1[] = {1, 4, 7, 10};
    int s2[] = {2, 5, 8};
    int s3[] = {3, 6, 9, 11, 12};

    stream_iter_t iters[3];
    IterHeap h = { .arr = iters, .num = 0, .size = 3 };

    h.arr[h.num] = (stream_iter_t){s1, s1 + 4}; push_iter(&h);
    h.arr[h.num] = (stream_iter_t){s2, s2 + 3}; push_iter(&h);
    h.arr[h.num] = (stream_iter_t){s3, s3 + 5}; push_iter(&h);

    macro_kway_merge_loop(stream_iter_t, &h, pop_iter, advance_iter, process_iter);

    MACRO_ASSERT_EQ_INT(output_count, 12);
    for (int i = 0; i < output_count; i++) {
        MACRO_ASSERT_EQ_INT(merged_output[i], i + 1);
    }
}

int main(void) {
    macro_test_case tests[64];
    size_t test_count = 0;

    MACRO_ADD(tests, kway_merge_basic_three_streams);

    macro_run_all("the-macro-library/macro_kway_merge", tests, test_count);
    return 0;
}
