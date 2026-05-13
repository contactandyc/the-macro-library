// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

/* --------------------------------------------------------------------------
 * EXACT MATCH FIND
 * Returns the first match found at any height. Perfect for standard maps.
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_find_code_with_field(head, field, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else if (n < 0) { \
                break; \
            } else { \
                return val_next; \
            } \
        } \
    } \
    return NULL;

#define _macro_skiplist_find_code(head, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else if (n < 0) { \
                break; \
            } else { \
                return val_next; \
            } \
        } \
    } \
    return NULL;

#define _macro_skiplist_kv_find_code_with_field(head, field, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else if (n < 0) { \
                break; \
            } else { \
                return val_next; \
            } \
        } \
    } \
    return NULL;

#define _macro_skiplist_kv_find_code(head, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else if (n < 0) { \
                break; \
            } else { \
                return val_next; \
            } \
        } \
    } \
    return NULL;

/* --------------------------------------------------------------------------
 * LOWER BOUND FIND (Crucial for MVCC and Multi-map lookups)
 * Drops all the way to level 0 to guarantee returning the absolutely FIRST
 * element >= the requested key.
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_lower_bound_code_with_field(head, field, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return macro_parent_object(target, value_type, field); \
    } \
    return NULL;

#define _macro_skiplist_lower_bound_code(head, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return (value_type *)target; \
    } \
    return NULL;

#define _macro_skiplist_kv_lower_bound_code_with_field(head, field, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return macro_parent_object(target, value_type, field); \
    } \
    return NULL;

#define _macro_skiplist_kv_lower_bound_code(head, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return (value_type *)target; \
    } \
    return NULL;

/* --------------------------------------------------------------------------
 * INSERTS
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_insert_code_with_field(head, field, style, value_type, cmp, node) \
    macro_skiplist_t *update[256]; \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp(style, value_type, cmp, node, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
            } else if (n < 0) { \
                break; \
            } else { \
                return false; /* Unique Map: Reject Duplicates */ \
            } \
        } \
        update[i] = x; \
    } \
    macro_skiplist_t *sl_node = &(node)->field; \
    int h = sl_node->height; \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&sl_node->forward[i], \
            MACRO_ATOMIC_LOAD(&update[i]->forward[i], relaxed), relaxed); \
    } \
    MACRO_ATOMIC_FENCE(release); \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&update[i]->forward[i], sl_node, release); \
    } \
    return true;

#define _macro_skiplist_insert_code(head, style, value_type, cmp, node) \
    macro_skiplist_t *update[256]; \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp(style, value_type, cmp, node, val_next); \
            if (n > 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
            } else if (n < 0) { \
                break; \
            } else { \
                return false; \
            } \
        } \
        update[i] = x; \
    } \
    macro_skiplist_t *sl_node = (macro_skiplist_t *)(node); \
    int h = sl_node->height; \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&sl_node->forward[i], \
            MACRO_ATOMIC_LOAD(&update[i]->forward[i], relaxed), relaxed); \
    } \
    MACRO_ATOMIC_FENCE(release); \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&update[i]->forward[i], sl_node, release); \
    } \
    return true;

/* --------------------------------------------------------------------------
 * MULTIMAP INSERTS (Allows duplicate keys by advancing on equality)
 * -------------------------------------------------------------------------- */

#define _macro_multimap_skiplist_insert_code_with_field(head, field, style, value_type, cmp, node) \
    macro_skiplist_t *update[256]; \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp(style, value_type, cmp, node, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
            } else { \
                break; \
            } \
        } \
        update[i] = x; \
    } \
    macro_skiplist_t *sl_node = &(node)->field; \
    int h = sl_node->height; \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&sl_node->forward[i], \
            MACRO_ATOMIC_LOAD(&update[i]->forward[i], relaxed), relaxed); \
    } \
    MACRO_ATOMIC_FENCE(release); \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&update[i]->forward[i], sl_node, release); \
    } \
    return true;

#define _macro_multimap_skiplist_insert_code(head, style, value_type, cmp, node) \
    macro_skiplist_t *update[256]; \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp(style, value_type, cmp, node, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], relaxed); \
            } else { \
                break; \
            } \
        } \
        update[i] = x; \
    } \
    macro_skiplist_t *sl_node = (macro_skiplist_t *)(node); \
    int h = sl_node->height; \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&sl_node->forward[i], \
            MACRO_ATOMIC_LOAD(&update[i]->forward[i], relaxed), relaxed); \
    } \
    MACRO_ATOMIC_FENCE(release); \
    for (int i = 0; i < h; i++) { \
        MACRO_ATOMIC_STORE(&update[i]->forward[i], sl_node, release); \
    } \
    return true;


/* --------------------------------------------------------------------------
 * UPPER BOUND FIND (First element strictly > key)
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_upper_bound_code_with_field(head, field, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return macro_parent_object(target, value_type, field); \
    } \
    return NULL;

#define _macro_skiplist_upper_bound_code(head, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return (value_type *)target; \
    } \
    return NULL;

#define _macro_skiplist_kv_upper_bound_code_with_field(head, field, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return macro_parent_object(target, value_type, field); \
    } \
    return NULL;

#define _macro_skiplist_kv_upper_bound_code(head, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    macro_skiplist_t *target = MACRO_ATOMIC_LOAD(&x->forward[0], acquire); \
    if (target) { \
        return (value_type *)target; \
    } \
    return NULL;

/* --------------------------------------------------------------------------
 * FLOOR FIND (Last element <= key)
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_floor_code_with_field(head, field, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    if (x != (macro_skiplist_t *)(head)) { \
        return macro_parent_object(x, value_type, field); \
    } \
    return NULL;

#define _macro_skiplist_floor_code(head, style, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp(style, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    if (x != (macro_skiplist_t *)(head)) { \
        return (value_type *)x; \
    } \
    return NULL;

#define _macro_skiplist_kv_floor_code_with_field(head, field, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = macro_parent_object(next, value_type, field); \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    if (x != (macro_skiplist_t *)(head)) { \
        return macro_parent_object(x, value_type, field); \
    } \
    return NULL;

#define _macro_skiplist_kv_floor_code(head, style, key_type, value_type, cmp, key) \
    macro_skiplist_t *x = (macro_skiplist_t *)(head); \
    int max_h = x->height; \
    for (int i = max_h - 1; i >= 0; i--) { \
        macro_skiplist_t *next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
        while (next) { \
            value_type *val_next = (value_type *)next; \
            int n = macro_cmp_kv(style, key_type, value_type, cmp, key, val_next); \
            if (n >= 0) { \
                x = next; \
                next = MACRO_ATOMIC_LOAD(&x->forward[i], acquire); \
            } else { \
                break; \
            } \
        } \
    } \
    if (x != (macro_skiplist_t *)(head)) { \
        return (value_type *)x; \
    } \
    return NULL;

/* --------------------------------------------------------------------------
 * CEILING FIND (First element >= key)
 * Functionally identical to lower_bound. Aliased for semantic API completeness.
 * -------------------------------------------------------------------------- */
#define _macro_skiplist_ceiling_code_with_field       _macro_skiplist_lower_bound_code_with_field
#define _macro_skiplist_ceiling_code                  _macro_skiplist_lower_bound_code
#define _macro_skiplist_kv_ceiling_code_with_field    _macro_skiplist_kv_lower_bound_code_with_field
#define _macro_skiplist_kv_ceiling_code               _macro_skiplist_kv_lower_bound_code
