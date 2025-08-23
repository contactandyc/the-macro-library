// SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#ifndef _macro_string_map_H
#define _macro_string_map_H

#include "the-macro-library/macro_map.h"

#include <string.h>

static inline
int compare_macro_string_map_for_insert(const macro_map_t *p1, const macro_map_t *p2) {
    const char *a = (const char *)(p1+1);
    const char *b = (const char *)(p2+1);
    return strcasecmp(a, b);
}

static inline
int compare_macro_string_map_for_find(const char *a, const macro_map_t *p2) {
    const char *b = (const char *)(p2+1);
    return strcasecmp(a, b);
}

static inline macro_map_insert(macro_string_insert, macro_map_t, compare_macro_string_map_for_insert);
// void macro_string_insert(macro_map_t **root, macro_map_t *item);
static inline macro_map_find_kv(macro_string_find, char, macro_map_t, compare_macro_string_map_for_find);
// macro_map_t* macro_string_find(const macro_map_t *root, const char *key);

#endif
