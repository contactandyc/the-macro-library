// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_union_find_H
#define _macro_union_find_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "the-macro-library/macro_parent_object.h"


/* --------------------------------------------------------------------------
 * INTRUSIVE LINK STRUCT
 * Embed this inside your concrete node type.
 * -------------------------------------------------------------------------- */
typedef struct macro_uf_s {
    struct macro_uf_s *parent;
    uint32_t rank;
} macro_uf_t;

/* Initialize a node to be its own root */
static inline void macro_uf_init_node(macro_uf_t *node) {
    node->parent = node;
    node->rank = 0;
}

/* --------------------------------------------------------------------------
 * CORE ALGORITHMS (Type-Agnostic)
 * -------------------------------------------------------------------------- */

/* Finds the root of the set, applying single-pass Path Halving */
static inline macro_uf_t *_macro_uf_find(macro_uf_t *node) {
    while (node != node->parent) {
        node->parent = node->parent->parent; // Path Halving!
        node = node->parent;
    }
    return node;
}

/* Unifies two sets by rank. Returns the resulting root. */
static inline macro_uf_t *_macro_uf_union(macro_uf_t *n1, macro_uf_t *n2) {
    macro_uf_t *root1 = _macro_uf_find(n1);
    macro_uf_t *root2 = _macro_uf_find(n2);

    if (root1 == root2) return root1;

    if (root1->rank < root2->rank) {
        root1->parent = root2;
        return root2;
    } else if (root1->rank > root2->rank) {
        root2->parent = root1;
        return root1;
    } else {
        root2->parent = root1;
        root1->rank++;
        return root1;
    }
}

/* --------------------------------------------------------------------------
 * MACRO GENERATORS (Type-Safe Wrappers)
 * -------------------------------------------------------------------------- */

/* Generates `type * name_find(type *node)` */
#define macro_uf_find_h(name, type) \
    static inline type * name##_find(type *node)

#define macro_uf_find_with_field(name, field, type) \
    macro_uf_find_h(name, type) { \
        if (!node) return NULL; \
        macro_uf_t *root = _macro_uf_find(&node->field); \
        return macro_parent_object(root, type, field); \
    }

#define macro_uf_find(name, type) \
    macro_uf_find_h(name, type) { \
        if (!node) return NULL; \
        return (type *)_macro_uf_find((macro_uf_t *)node); \
    }

/* Generates `type * name_union(type *n1, type *n2)` */
#define macro_uf_union_h(name, type) \
    static inline type * name##_union(type *n1, type *n2)

#define macro_uf_union_with_field(name, field, type) \
    macro_uf_union_h(name, type) { \
        if (!n1) return n2; \
        if (!n2) return n1; \
        macro_uf_t *root = _macro_uf_union(&n1->field, &n2->field); \
        return macro_parent_object(root, type, field); \
    }

#define macro_uf_union(name, type) \
    macro_uf_union_h(name, type) { \
        if (!n1) return n2; \
        if (!n2) return n1; \
        return (type *)_macro_uf_union((macro_uf_t *)n1, (macro_uf_t *)n2); \
    }

/* Generates `bool name_connected(type *n1, type *n2)` */
#define macro_uf_connected_h(name, type) \
    static inline bool name##_connected(type *n1, type *n2)

#define macro_uf_connected_with_field(name, field, type) \
    macro_uf_connected_h(name, type) { \
        if (!n1 || !n2) return false; \
        return _macro_uf_find(&n1->field) == _macro_uf_find(&n2->field); \
    }

#define macro_uf_connected(name, type) \
    macro_uf_connected_h(name, type) { \
        if (!n1 || !n2) return false; \
        return _macro_uf_find((macro_uf_t *)n1) == _macro_uf_find((macro_uf_t *)n2); \
    }

/* Helper to generate all three functions at once */
#define macro_union_find_with_field(name, field, type) \
    macro_uf_find_with_field(name, field, type) \
    macro_uf_union_with_field(name, field, type) \
    macro_uf_connected_with_field(name, field, type)

#define macro_union_find(name, type) \
    macro_uf_find(name, type) \
    macro_uf_union(name, type) \
    macro_uf_connected(name, type)

#endif /* _macro_union_find_H */
