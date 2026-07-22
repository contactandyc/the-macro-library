// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _MACRO_KWAY_MERGE_H
#define _MACRO_KWAY_MERGE_H

#include "the-macro-library/macro_heap.h"

/*
 * Generates a K-Way merge loop.
 * - `type`: The struct type stored in the heap array (e.g., your iterator struct).
 * - `heap`: A populated `macro_min_heap` struct.
 * - `heap_pop_fn`: The generated pop macro (e.g., `min_heap_pop_myiter`).
 * - `advance_fn`: A function/macro: `void advance(type *iter, heap_t *h)`.
 * - `process_fn`: A function/macro: `void process(type *iter)`.
 */
#define macro_kway_merge_loop(type, heap, heap_pop_fn, advance_fn, process_fn) do { \
    while ((heap)->num > 0) { \
        type *min_item = heap_pop_fn(heap); \
        if (!min_item) break; \
        type current = *min_item; \
        process_fn(&current); \
        advance_fn(&current, heap); \
    } \
} while(0)

#endif /* _MACRO_KWAY_MERGE_H */
