// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_map_H
#define _macro_map_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

// Important: These APIs assume macro_map_t is the first member of your value type. If it is not, use the
// _with_field variants.

#include "the-macro-library/macro_parent_object.h"
#include "the-macro-library/macro_cmp.h"

#ifndef STATIC_ASSERT
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#    define STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)
#  else
     /* C99 fallback */
#    define STATIC_ASSERT(cond, msg) typedef char static_assertion_##__LINE__[(cond)?1:-1]
#  endif
#endif

#ifndef UINTPTR_MAX
/* Fallback: assume size_t can carry a pointer (common but not guaranteed) */
typedef size_t uintptr_t;
#endif

STATIC_ASSERT(sizeof(uintptr_t) >= sizeof(void*), "uintptr_t too small");

#if defined(__GNUC__) || defined(__clang__)
  /* Attribute goes AFTER the typedef name */
  typedef struct macro_map_s {
      uintptr_t parent_color;   /* was size_t */
      struct macro_map_s *left;
      struct macro_map_s *right;
  } macro_map_t __attribute__((aligned(2)));
#elif defined(_MSC_VER)
  /* declspec goes BEFORE the type name */
  __declspec(align(2))
  typedef struct macro_map_s {
      uintptr_t parent_color;
      struct macro_map_s *left;
      struct macro_map_s *right;
  } macro_map_t;
#else
  /* No attribute: rely on the static assert below to catch problems */
  typedef struct macro_map_s {
      uintptr_t parent_color;
      struct macro_map_s *left;
      struct macro_map_s *right;
  } macro_map_t;
#endif

/* The alignment is needed because the color uses the lowest bit */

#if __STDC_VERSION__ >= 201112L
_Static_assert(_Alignof(macro_map_t) % 2 == 0,
               "macro_map_t must be >=2 aligned for LSB color tagging");
#endif

/* iteration */
static inline macro_map_t *macro_map_first(macro_map_t *n);
static inline macro_map_t *macro_map_last(macro_map_t *n);
static inline macro_map_t *macro_map_next(macro_map_t *n);
static inline macro_map_t *macro_map_previous(macro_map_t *n);
static inline macro_map_t *macro_map_postorder_first(macro_map_t *n);
static inline macro_map_t *macro_map_postorder_next(macro_map_t *n);

/*
  macro_map_copy_node_cb is a callback meant to be used with macro_map_copy to
  copy one map to another given a root node.  The arg will typically be an
  allocator such as the pool.
*/
typedef macro_map_t *(*macro_map_copy_node_cb)(macro_map_t *n, void *arg);

static inline macro_map_t *macro_map_copy(macro_map_t *root, macro_map_copy_node_cb copy, void *arg);

static inline bool macro_map_erase(macro_map_t **root, macro_map_t *node);

static inline void _macro_map_fix_insert(macro_map_t *node, macro_map_t *parent, macro_map_t **root);

#define macro_map_color(n)            ((n)->parent_color & (uintptr_t)1)
#define _macro_map_is_red(n)          ((((n)->parent_color & (uintptr_t)1) == 0))
#define _macro_map_is_black(n)        ((((n)->parent_color & (uintptr_t)1) == 1))

#define _macro_map_clear_black(n)     ((n)->parent_color = (uintptr_t)1)

#define _macro_map_set_black(n)  ((n)->parent_color |=  (uintptr_t)1)
#define _macro_map_set_red(n)    ((n)->parent_color &= ~(uintptr_t)1)
#define _macro_map_parent(n)     ((macro_map_t *)((n)->parent_color & ~(uintptr_t)1))
#define _macro_map_set_parent(n, parent)    \
    ((n)->parent_color = ((n)->parent_color & (uintptr_t)1) | (uintptr_t)(parent))

/* iteration */
static inline macro_map_t *macro_map_first(macro_map_t *n) {
    if (!n)
        return NULL;
    while (n->left)
        n = n->left;
    return n;
}

static inline macro_map_t *macro_map_last(macro_map_t *n) {
    if (!n)
        return NULL;
    while (n->right)
        n = n->right;
    return n;
}

static inline macro_map_t *macro_map_next(macro_map_t *n) {
    if (n->right) {
        n = n->right;
        while (n->left)
            n = n->left;
        return n;
    }
    macro_map_t *parent = _macro_map_parent(n);
    while (parent && parent->right == n) {
        n = parent;
        parent = _macro_map_parent(n);
    }
    return parent;
}

static inline macro_map_t *macro_map_previous(macro_map_t *n) {
    if (n->left) {
        n = n->left;
        while (n->right)
            n = n->right;
        return n;
    }
    macro_map_t *parent = _macro_map_parent(n);
    while (parent && parent->left == n) {
        n = parent;
        parent = _macro_map_parent(n);
    }
    return parent;
}

static inline macro_map_t *_macro_map_left_deepest_node(macro_map_t *n) {
    while (true) {
        if (n->left)
            n = n->left;
        else if (n->right)
            n = n->right;
        else
            return n;
    }
}

static inline macro_map_t *macro_map_postorder_first(macro_map_t *n) {
    if (!n)
        return NULL;
    return _macro_map_left_deepest_node(n);
}

static inline macro_map_t *macro_map_postorder_next(macro_map_t *n) {
    macro_map_t *parent = _macro_map_parent(n);
    if (parent && n == parent->left && parent->right)
        return _macro_map_left_deepest_node(parent->right);
    else
        return parent;
}

/* copy */
static inline void _macro_map_tree_copy(macro_map_t *n, macro_map_t **res, macro_map_t *parent,
                                        macro_map_copy_node_cb copy, void *arg) {
    macro_map_t *c = copy(n, arg);
    *res = c;
    c->parent_color = n->parent_color;
    _macro_map_set_parent(c, parent);
    if (n->left)
        _macro_map_tree_copy(n->left, &c->left, c, copy, arg);
    else
        c->left = NULL;
    if (n->right)
        _macro_map_tree_copy(n->right, &c->right, c, copy, arg);
    else
        c->right = NULL;
}

static inline macro_map_t *macro_map_copy(macro_map_t *root, macro_map_copy_node_cb copy, void *arg) {
    macro_map_t *res = NULL;
    if (root)
        _macro_map_tree_copy(root, &res, NULL, copy, arg);
    return res;
}
static inline void _macro_map_rotate_left(macro_map_t *A, macro_map_t **root) {
    macro_map_t *new_root = A->right;

    uintptr_t tmp_pc = A->parent_color;                 // was size_t
    A->parent_color = new_root->parent_color;
    new_root->parent_color = tmp_pc;

    macro_map_t *parent = _macro_map_parent(new_root);
    if (parent) {
        if (parent->left == A) parent->left = new_root; else parent->right = new_root;
    } else if (root) {
        *root = new_root;                               // guarded write
    }

    macro_map_t *tmp = new_root->left;
    new_root->left = A;
    _macro_map_set_parent(A, new_root);

    A->right = tmp;
    if (tmp) _macro_map_set_parent(tmp, A);
}

static inline void _macro_map_rotate_right(macro_map_t *A, macro_map_t **root) {
    macro_map_t *new_root = A->left;

    uintptr_t tmp_pc = A->parent_color;
    A->parent_color = new_root->parent_color;
    new_root->parent_color = tmp_pc;

    macro_map_t *parent = _macro_map_parent(new_root);
    if (parent) {
        if (parent->left == A) parent->left = new_root; else parent->right = new_root;
    } else if (root) {
        *root = new_root;
    }

    macro_map_t *tmp = new_root->right;
    new_root->right = A;
    _macro_map_set_parent(A, new_root);

    A->left = tmp;
    if (tmp) _macro_map_set_parent(tmp, A);
}


static inline void _macro_map_fix_insert(macro_map_t *node, macro_map_t *parent, macro_map_t **root) {
    _macro_map_set_red(node);
    _macro_map_set_parent(node, parent);
    node->left = node->right = NULL;

    macro_map_t *grandparent, *uncle;

    while (true) {
        parent = _macro_map_parent(node);
        if (!parent) {
            _macro_map_clear_black(node);
            break;
        }

        if (_macro_map_is_black(parent))
            break;

        grandparent = _macro_map_parent(parent);
        if (grandparent->left == parent) {
            uncle = grandparent->right;
            if (uncle && _macro_map_is_red(uncle)) {
                _macro_map_set_red(grandparent);
                _macro_map_set_black(parent);
                _macro_map_set_black(uncle);
                node = grandparent;
                continue;
            }
            if (parent->right == node)
                _macro_map_rotate_left(parent, NULL);
            _macro_map_rotate_right(grandparent, root);
            break;
        } else {
            uncle = grandparent->left;
            if (uncle && _macro_map_is_red(uncle)) {
                _macro_map_set_red(grandparent);
                _macro_map_set_black(parent);
                _macro_map_set_black(uncle);
                node = grandparent;
                continue;
            }
            if (parent->left == node)
                _macro_map_rotate_right(parent, NULL);
            _macro_map_rotate_left(grandparent, root);
            break;
        }
    }
}

static inline
void _fix_color_for_erase(macro_map_t *parent, macro_map_t *node,
                          macro_map_t **root) {
    macro_map_t *sibling;
    if (parent->right != node) {
        sibling = parent->right;
        if (_macro_map_is_red(sibling)) {
            _macro_map_rotate_left(parent, root);
            sibling = parent->right;
        }
        if (sibling->right && _macro_map_is_red(sibling->right)) {
            _macro_map_set_black(sibling->right);
            _macro_map_rotate_left(parent, root);
        } else if (sibling->left && _macro_map_is_red(sibling->left)) {
            _macro_map_rotate_right(sibling, root);
            _macro_map_rotate_left(parent, root);
            _macro_map_set_black(sibling);
        } else {
            _macro_map_set_red(sibling);
            if (_macro_map_parent(parent) && _macro_map_is_black(parent))
                _fix_color_for_erase(_macro_map_parent(parent), parent, root);
            else
                _macro_map_set_black(parent);
        }
    } else {
        sibling = parent->left;
        if (_macro_map_is_red(sibling)) {
            _macro_map_rotate_right(parent, root);
            sibling = parent->left;
        }
        if (sibling->left && _macro_map_is_red(sibling->left)) {
            _macro_map_set_black(sibling->left);
            _macro_map_rotate_right(parent, root);
        } else if (sibling->right && _macro_map_is_red(sibling->right)) {
            _macro_map_rotate_left(sibling, root);
            _macro_map_rotate_right(parent, root);
            _macro_map_set_black(sibling);
        } else {
            _macro_map_set_red(sibling);
            if (_macro_map_parent(parent) && _macro_map_is_black(parent))
                _fix_color_for_erase(_macro_map_parent(parent), parent, root);
            else
                _macro_map_set_black(parent);
        }
    }
}

static inline void _replace_node_with_child(macro_map_t *child, macro_map_t *node,
                                           macro_map_t **root) {
    macro_map_t *parent = _macro_map_parent(node);
    if (parent) {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    } else
        *root = child;

    child->parent_color = node->parent_color;
}

static inline bool macro_map_erase(macro_map_t **root, macro_map_t *node) {
    macro_map_t *parent = _macro_map_parent(node);
    if (!node->left) {
        if (node->right)
            _replace_node_with_child(node->right, node, root);
        else {
            if (parent) {
                if (parent->left == node)
                    parent->left = NULL;
                else
                    parent->right = NULL;
                if (_macro_map_is_black(node))
                    _fix_color_for_erase(parent, NULL, root);
            } else
                *root = NULL;
        }
    } else if (!node->right)
        _replace_node_with_child(node->left, node, root);
    else {
        macro_map_t *successor = node->right;
        if (!successor->left) {
            bool black = _macro_map_is_black(successor);
            _replace_node_with_child(successor, node, root);
            successor->left = node->left;
            _macro_map_set_parent(successor->left, successor);
            if (successor->right)
                _macro_map_set_black(successor->right);
            else {
                if (black)
                    _fix_color_for_erase(successor, NULL, root);
            }
        } else {
            while (successor->left)
                successor = successor->left;

            bool black = _macro_map_is_black(successor);
            macro_map_t *right = successor->right;
            macro_map_t *parent = _macro_map_parent(successor);
            parent->left = right;
            if (right) {
                _macro_map_clear_black(right);
                _macro_map_set_parent(right, parent);
                black = false;
            }
            _replace_node_with_child(successor, node, root);
            successor->left = node->left;
            _macro_map_set_parent(successor->left, successor);
            successor->right = node->right;
            _macro_map_set_parent(successor->right, successor);
            if (black)
                _fix_color_for_erase(parent, NULL, root);
        }
    }
    return true;
}


#define _macro_map_find_code_head(value_type)

#define _macro_map_find_code_inner(value)    \
    if (n < 0)                               \
        root = root->left;                   \
    else if (n > 0)                          \
        root = root->right;                  \
    else                                     \
        return value;                        \

#define _macro_map_find_code_tail(key, style, type, cmp)
#define _macro_map_kv_find_code_tail(key, style, key_type, value_type, cmp)

/* first */

#define _macro_map_first_code_head(value_type)    \
    value_type *res = NULL;

#define _macro_map_first_code_inner(value)    \
      if (n <= 0) {                           \
        res = value;                          \
        root = root->left;                    \
      } else                                  \
        root = root->right;                   \

#define _macro_map_first_code_tail(key, style, type, cmp)    \
    if (res && macro_equal(style,type, cmp, key, res))       \
      return res;                                            \
    return NULL;

#define _macro_map_kv_first_code_tail(key, style, key_type, value_type, cmp)    \
    if (res && macro_equal_kv(style, key_type, value_type, cmp, key, res))      \
      return res;                                                               \
    return NULL;

/* last */
#define _macro_map_last_code_head(value_type) _macro_map_first_code_head(value_type)

#define _macro_map_last_code_inner(value)    \
    if (n < 0)                               \
        root = root->left;                   \
    else {                                   \
        res = value;                         \
        root = root->right;                  \
    }

#define _macro_map_last_code_tail(key, style, type, cmp) _macro_map_first_code_tail(key, style, type, cmp)

#define _macro_map_kv_last_code_tail(key, style, key_type, value_type, cmp)    \
    _macro_map_kv_first_code_tail(key, style, key_type, value_type, cmp)

/* lower_bound */
#define _macro_map_lower_bound_code_head(value_type) _macro_map_first_code_head(value_type)
#define _macro_map_lower_bound_code_inner(value) _macro_map_first_code_inner(value)
#define _macro_map_lower_bound_code_tail(key, style, type, cmp) return res;
#define _macro_map_kv_lower_bound_code_tail(key, style, key_type, value_type, cmp) return res;

/* upper_bound */
#define _macro_map_upper_bound_code_head(value_type) _macro_map_first_code_head(value_type)

#define _macro_map_upper_bound_code_inner(value)    \
    if (n < 0) {                                    \
        res = value;                                \
        root = root->left;                          \
    } else                                          \
        root = root->right;

#define _macro_map_upper_bound_code_tail(key, style, type, cmp)    \
    _macro_map_lower_bound_code_tail(key, style, type, cmp)
#define _macro_map_kv_upper_bound_code_tail(key, style, key_type, value_type, cmp)    \
    _macro_map_kv_lower_bound_code_tail(key, style, key_type, value_type, cmp)

/* floor */
#define _macro_map_floor_code_head(value_type)    \
    value_type *res = NULL, *equal = NULL;

#define _macro_map_floor_code_inner(value)    \
    if (n < 0) {                              \
        root = root->left;                    \
    } else if (n > 0) {                       \
        res = value;                          \
        root = root->right;                   \
    } else {                                  \
        equal = value;                        \
        root = root->right;                   \
    }

#define _macro_map_floor_code_tail(key, style, type, cmp) return equal ? equal : res;
#define _macro_map_kv_floor_code_tail(key, style, key_type, value_type, cmp) return equal ? equal : res;

/* ceiling */
#define _macro_map_ceiling_code_head(value_type)    \
    _macro_map_floor_code_head(value_type)

#define _macro_map_ceiling_code_inner(value)    \
    if (n > 0) {                                \
        root = root->right;                     \
    } else if (n < 0) {                         \
        res = value;                            \
        root = root->left;                      \
    } else {                                    \
        equal = value;                          \
        root = root->left;                      \
    }

#define _macro_map_ceiling_code_tail(key, style, type, cmp)    \
    _macro_map_floor_code_tail(key, style, type, cmp)

#define _macro_map_kv_ceiling_code_tail(key, style, key_type, value_type, cmp)    \
    _macro_map_kv_floor_code_tail(key, style, key_type, value_type, cmp)

/* end of custom find functions */

#define _macro_map_code_with_field(root, field, find_style, style, value_type, cmp, key )    \
    _macro_map_ ## find_style ## _code_head(value_type)                                      \
    while (root) {                                                                           \
        value_type *value = macro_parent_object(root, value_type, field );                   \
        int n = macro_cmp(style, value_type, cmp, key, value);                               \
        _macro_map_ ## find_style ## _code_inner(value)                                      \
    }                                                                                        \
    _macro_map_ ## find_style ## _code_tail(key, style, value_type, cmp)                     \
    return NULL;

#define _macro_map_code(root, find_style, style, value_type, cmp, key )     \
    _macro_map_ ## find_style ## _code_head(value_type)                     \
    while (root) {                                                          \
        int n = macro_cmp(style, value_type, cmp, key, root);               \
        _macro_map_ ## find_style ## _code_inner((value_type *)root)        \
    }                                                                       \
    _macro_map_ ## find_style ## _code_tail(key, style, value_type, cmp)    \
    return NULL;

#define _macro_map_kv_code_with_field(root, field, find_style, style, key_type, value_type, cmp, key )    \
    _macro_map_ ## find_style ## _code_head(value_type)                                                   \
    while (root) {                                                                                        \
        value_type *value = macro_parent_object(root, value_type, field );                                \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, value);                               \
        _macro_map_ ## find_style ## _code_inner(value)                                                   \
    }                                                                                                     \
    _macro_map_kv_ ## find_style ## _code_tail(key, style, key_type, value_type, cmp)                     \
    return NULL;

#define _macro_map_kv_code(root, find_style, style, key_type, value_type, cmp, key )     \
    _macro_map_ ## find_style ## _code_head(value_type)                                  \
    while (root) {                                                                       \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, root);               \
        _macro_map_ ## find_style ## _code_inner((value_type *)root)                     \
    }                                                                                    \
    _macro_map_kv_ ## find_style ## _code_tail(key, style, key_type, value_type, cmp)    \
    return NULL;

#define _macro_map_h(name, style, type)    \
    type *name(const macro_map_t *root, macro_cmp_signature(const type *key, style, type))

#define _macro_map(name, find_style, style, type, cmp)               \
    _macro_map_h(name, style, type) {                                \
        _macro_map_code(root, find_style, style, type, cmp, key);    \
    }

#define _macro_map_with_field(name, field, find_style, style, type, cmp)               \
    _macro_map_h(name, style, type) {                                                  \
        _macro_map_code_with_field(root, field, find_style, style, type, cmp, key);    \
    }

#define _macro_map_compare_h(name, style, type) _macro_map_h(name, compare_ ## style, type)

#define _macro_map_compare(name, find_style, style, type)            \
    _macro_map_compare_h(name, style, type) {                        \
        _macro_map_code(root, find_style, style, type, cmp, key);    \
    }

#define _macro_map_compare_with_field(name, field, find_style, style, type)            \
    _macro_map_compare_h(name, style, type) {                                          \
        _macro_map_code_with_field(root, field, find_style, style, type, cmp, key);    \
    }

#define _macro_map_kv_h(name, style, key_type, value_type)    \
    value_type *name(const macro_map_t *root, macro_cmp_kv_signature(const key_type *key, style, key_type, value_type))

#define _macro_map_kv(name, find_style, style, key_type, value_type, cmp)               \
    _macro_map_kv_h(name, style, key_type, value_type) {                                \
        _macro_map_kv_code(root, find_style, style, key_type, value_type, cmp, key);    \
    }

#define _macro_map_kv_with_field(name, field, find_style, style, key_type, value_type, cmp)               \
    _macro_map_kv_h(name, style, key_type, value_type) {                                                  \
        _macro_map_kv_code_with_field(root, field, find_style, style, key_type, value_type, cmp, key);    \
    }

#define _macro_map_kv_compare_h(name, style, key_type, value_type) _macro_map_kv_h(name, compare_ ## style, key_type, value_type)

#define _macro_map_kv_compare(name, find_style, style, key_type, value_type)            \
    _macro_map_kv_compare_h(name, style, key_type, value_type) {                        \
        _macro_map_kv_code(root, find_style, style, key_type, value_type, cmp, key);    \
    }

#define _macro_map_kv_compare_with_field(name, field, find_style, style, key_type, value_type)            \
    _macro_map_kv_compare_h(name, style, key_type, value_type) {                                          \
        _macro_map_kv_code_with_field(root, field, find_style, style, key_type, value_type, cmp, key);    \
    }

#define _macro_map_insert_code_with_field(root, field, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                                             \
    while (*np) {                                                                        \
        parent = *np;                                                                    \
        value_type *value = macro_parent_object(parent, value_type, field );             \
        int n = macro_cmp(style, value_type, cmp, node, value);                          \
        if(n < 0)                                                                        \
            np = &(parent->left);                                                        \
        else if (n > 0)                                                                  \
            np = &(parent->right);                                                       \
        else                                                                             \
            return false;                                                                \
    }                                                                                    \
    *np = &node->field;                                                                  \
    _macro_map_fix_insert(*np, parent, root);                                            \
    return true;

#define _macro_map_insert_code(root, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                           \
    while (*np) {                                                      \
        parent = *np;                                                  \
        int n = macro_cmp(style, value_type, cmp, node, parent);       \
        if(n < 0)                                                      \
            np = &(parent->left);                                      \
        else if (n > 0)                                                \
            np = &(parent->right);                                     \
        else                                                           \
            return false;                                              \
    }                                                                  \
    *np = (macro_map_t *)node;                                         \
    _macro_map_fix_insert(*np, parent, root);                          \
    return true;

#define _macro_multimap_insert_code_with_field(root, field, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                                                  \
    while (*np) {                                                                             \
        parent = *np;                                                                         \
        value_type *value = macro_parent_object(parent, value_type, field );                  \
        int n = macro_cmp(style, value_type, cmp, node, value);                               \
        if(n < 0)                                                                             \
            np = &(parent->left);                                                             \
        else if (n > 0)                                                                       \
            np = &(parent->right);                                                            \
        else {                                                                                \
            if (node < value)                                                                 \
                np = &(parent->left);                                                         \
            else if (node > value)                                                            \
                np = &(parent->right);                                                        \
            else                                                                              \
                return false;                                                                 \
        }                                                                                     \
    }                                                                                         \
    *np = &node->field;                                                                       \
    _macro_map_fix_insert(*np, parent, root);                                                 \
    return true;

#define _macro_multimap_insert_code(root, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                                \
    while (*np) {                                                           \
        parent = *np;                                                       \
        int n = macro_cmp(style, value_type, cmp, node, parent);            \
        if(n < 0)                                                           \
            np = &(parent->left);                                           \
        else if (n > 0)                                                     \
            np = &(parent->right);                                          \
        else {                                                              \
            if (node < (value_type *)parent)                                \
                np = &(parent->left);                                       \
            else if (node > (value_type *)parent)                           \
                np = &(parent->right);                                      \
            else                                                            \
                return false;                                               \
        }                                                                   \
    }                                                                       \
    *np = (macro_map_t *)node;                                              \
    _macro_map_fix_insert(*np, parent, root);                               \
    return true;

#define _macro_map_insert_h(name, style, type)    \
    bool name(macro_map_t **root, macro_cmp_signature(type *node, style, type))

#define _macro_map_insert_with_field(name, field, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                                       \
        _macro_map_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_map_insert(name, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                     \
        _macro_map_insert_code(root, style, type, cmp, node);    \
    }

#define _macro_map_insert_compare_h(name, style, type) _macro_map_insert_h(name, compare_ ## style, type)

#define _macro_map_insert_compare_with_field(name, field, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {                               \
        _macro_map_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_map_insert_compare(name, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {             \
        _macro_map_insert_code(root, style, type, cmp, node);    \
    }

/* multimap */
#define _macro_multimap_insert_with_field(name, field, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                                            \
        _macro_multimap_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_multimap_insert(name, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                          \
        _macro_multimap_insert_code(root, style, type, cmp, node);    \
    }

#define _macro_multimap_insert_compare_with_field(name, field, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {                                    \
        _macro_multimap_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_multimap_insert_compare(name, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {                  \
        _macro_multimap_insert_code(root, style, type, cmp, node);    \
    }


/* create a set of defaults - cmp_no_arg */
#define macro_map_default() cmp_no_arg

#define macro_map_find_with_field(name, field, type, cmp) _macro_map_with_field(name, field, find, macro_map_default(), type, cmp)
#define macro_map_find(name, type, cmp) _macro_map(name, find, macro_map_default(), type, cmp)

#define macro_map_find_compare_with_field(name, field, type) _macro_map_compare_with_field(name, field, find, macro_map_default(), type)
#define macro_map_find_compare(name, type) _macro_map_compare(name, find, macro_map_default(), type)

#define macro_map_find_kv_with_field(name, field, key_type, value_type, cmp)    \
    _macro_map_kv_with_field(name, field, find, macro_map_default(), key_type, value_type, cmp)
#define macro_map_find_kv(name, key_type, value_type, cmp)    \
    _macro_map_kv(name, find, macro_map_default(), key_type, value_type, cmp)

#define macro_map_find_kv_compare_with_field(name, field, key_type, value_type)    \
    _macro_map_kv_compare_with_field(name, field, find, macro_map_default(), key_type, value_type)
#define macro_map_find_kv_compare(name, key_type, value_type)    \
    _macro_map_kv_compare(name, find, macro_map_default(), key_type, value_type)

#define macro_map_insert_h(name, type) _macro_map_insert_h(name, macro_map_default(), type)
#define macro_map_insert_compare_h(name, type) _macro_map_insert_compare_h(name, macro_map_default(), type)

#define macro_map_insert_with_field(name, field, type, cmp)    \
    _macro_map_insert_with_field(name, field, macro_map_default(), type, cmp)

#define macro_map_insert(name, type, cmp) _macro_map_insert(name, macro_map_default(), type, cmp)

#define macro_map_insert_compare_with_field(name, field, type)    \
    _macro_map_insert_compare_with_field(name, field, macro_map_default(), type)

#define macro_map_insert_compare(name, type) _macro_map_insert_compare(name, macro_map_default(), type)

/* multimap */
#define _macro_multimap_insert_h(name, style, type) _macro_map_insert_h(name, style, type)
#define _macro_multimap_insert_compare_h(name, style, type) _macro_map_insert_compare_h(name, style, type)

#define macro_multimap_insert_h(name, type) _macro_multimap_insert_h(name, macro_map_default(), type)
#define macro_multimap_insert_compare_h(name, type) _macro_multimap_insert_compare_h(name, macro_map_default(), type)

#define macro_multimap_insert_with_field(name, field, type, cmp)    \
    _macro_multimap_insert_with_field(name, field, macro_map_default(), type, cmp)

#define macro_multimap_insert(name, type, cmp) _macro_multimap_insert(name, macro_map_default(), type, cmp)

#define macro_multimap_insert_compare_with_field(name, field, type)    \
    _macro_multimap_insert_compare_with_field(name, field, macro_map_default(), type)

#define macro_multimap_insert_compare(name, type) _macro_multimap_insert_compare(name, macro_map_default(), type)

/* --------------------------------------------------------------------------
 * REMOVE WRAPPERS (Node-based unlinking)
 * -------------------------------------------------------------------------- */
#define macro_map_remove_h(name, type) \
    static inline type * name(macro_map_t **root, type *node)

#define macro_map_remove_with_field(name, field, type) \
    macro_map_remove_h(name, type) { \
        if (node && macro_map_erase(root, &node->field)) return node; \
        return NULL; \
    }

#define macro_map_remove(name, type) \
    macro_map_remove_h(name, type) { \
        if (node && macro_map_erase(root, (macro_map_t *)node)) return node; \
        return NULL; \
    }
/* ==========================================================================
 * INTERNAL GENERATOR ENGINE (Linkage Parameterized)
 * ========================================================================== */

/* Standard Maps */
#define __macro_map_api_h(MOD, prefix, type) \
    MOD _macro_map_h(prefix##_find, macro_map_default(), type); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type); \
    MOD type * prefix##_erase(macro_map_t **root, const type *key)

#define __macro_map_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_map_h(prefix##_find, macro_map_default(), type); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type); \
    MOD type * prefix##_erase(macro_map_t **root, const type *key)

#define __macro_map_api(MOD, prefix, type, cmp) \
    MOD macro_map_find(prefix##_find, type, cmp) \
    MOD macro_map_insert(prefix##_insert, type, cmp) \
    MOD macro_map_remove(prefix##_remove, type) \
    MOD type * prefix##_erase(macro_map_t **root, const type *key) { \
        type *node = prefix##_find(*root, key); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

#define __macro_map_api_with_field(MOD, prefix, field, type, cmp) \
    MOD macro_map_find_with_field(prefix##_find, field, type, cmp) \
    MOD macro_map_insert_with_field(prefix##_insert, field, type, cmp) \
    MOD macro_map_remove_with_field(prefix##_remove, field, type) \
    MOD type * prefix##_erase(macro_map_t **root, const type *key) { \
        type *node = prefix##_find(*root, key); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

/* KV Maps */
#define __macro_map_kv_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_map_kv_h(prefix##_find, macro_map_default(), key_type, value_type); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type); \
    MOD value_type * prefix##_erase(macro_map_t **root, const key_type *key)

#define __macro_map_kv_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_map_kv_h(prefix##_find, macro_map_default(), key_type, value_type); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type); \
    MOD value_type * prefix##_erase(macro_map_t **root, const key_type *key)

#define __macro_map_kv_api(MOD, prefix, key_type, value_type, cmp) \
    MOD macro_map_find_kv(prefix##_find, key_type, value_type, cmp) \
    MOD macro_map_insert(prefix##_insert, value_type, cmp) \
    MOD macro_map_remove(prefix##_remove, value_type) \
    MOD value_type * prefix##_erase(macro_map_t **root, const key_type *key) { \
        value_type *node = prefix##_find(*root, key); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

#define __macro_map_kv_api_with_field(MOD, prefix, field, key_type, value_type, cmp) \
    MOD macro_map_find_kv_with_field(prefix##_find, field, key_type, value_type, cmp) \
    MOD macro_map_insert_with_field(prefix##_insert, field, value_type, cmp) \
    MOD macro_map_remove_with_field(prefix##_remove, field, value_type) \
    MOD value_type * prefix##_erase(macro_map_t **root, const key_type *key) { \
        value_type *node = prefix##_find(*root, key); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

/* Multimaps */
#define __macro_multimap_api_h(MOD, prefix, type) \
    MOD _macro_map_h(prefix##_lower_bound, macro_map_default(), type); \
    MOD type * prefix##_find(const macro_map_t *root, const type *key); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type)

#define __macro_multimap_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_map_h(prefix##_lower_bound, macro_map_default(), type); \
    MOD type * prefix##_find(const macro_map_t *root, const type *key); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type)

#define __macro_multimap_api(MOD, prefix, type, cmp) \
    MOD _macro_map(prefix##_lower_bound, lower_bound, macro_map_default(), type, cmp) \
    MOD type * prefix##_find(const macro_map_t *root, const type *key) { \
        type *node = prefix##_lower_bound(root, key); \
        if (node && macro_equal(macro_map_default(), type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert(prefix##_insert, type, cmp) \
    MOD macro_map_remove(prefix##_remove, type)

#define __macro_multimap_api_with_field(MOD, prefix, field, type, cmp) \
    MOD _macro_map_with_field(prefix##_lower_bound, field, lower_bound, macro_map_default(), type, cmp) \
    MOD type * prefix##_find(const macro_map_t *root, const type *key) { \
        type *node = prefix##_lower_bound(root, key); \
        if (node && macro_equal(macro_map_default(), type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert_with_field(prefix##_insert, field, type, cmp) \
    MOD macro_map_remove_with_field(prefix##_remove, field, type)

/* KV Multimaps */
#define __macro_multimap_kv_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_map_kv_h(prefix##_lower_bound, macro_map_default(), key_type, value_type); \
    MOD value_type * prefix##_find(const macro_map_t *root, const key_type *key); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type)

#define __macro_multimap_kv_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_map_kv_h(prefix##_lower_bound, macro_map_default(), key_type, value_type); \
    MOD value_type * prefix##_find(const macro_map_t *root, const key_type *key); \
    MOD _macro_map_insert_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type)

#define __macro_multimap_kv_api(MOD, prefix, key_type, value_type, cmp) \
    MOD _macro_map_kv(prefix##_lower_bound, lower_bound, macro_map_default(), key_type, value_type, cmp) \
    MOD value_type * prefix##_find(const macro_map_t *root, const key_type *key) { \
        value_type *node = prefix##_lower_bound(root, key); \
        if (node && macro_equal_kv(macro_map_default(), key_type, value_type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert(prefix##_insert, value_type, cmp) \
    MOD macro_map_remove(prefix##_remove, value_type)

#define __macro_multimap_kv_api_with_field(MOD, prefix, field, key_type, value_type, cmp) \
    MOD _macro_map_kv_with_field(prefix##_lower_bound, field, lower_bound, macro_map_default(), key_type, value_type, cmp) \
    MOD value_type * prefix##_find(const macro_map_t *root, const key_type *key) { \
        value_type *node = prefix##_lower_bound(root, key); \
        if (node && macro_equal_kv(macro_map_default(), key_type, value_type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert_with_field(prefix##_insert, field, value_type, cmp) \
    MOD macro_map_remove_with_field(prefix##_remove, field, value_type)

/* ==========================================================================
 * INTERNAL COMPARE API GENERATORS
 * ========================================================================== */

/* Standard Maps Compare */
#define __macro_map_compare_api_h(MOD, prefix, type) \
    MOD _macro_map_compare_h(prefix##_find, macro_map_default(), type); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type); \
    MOD type * prefix##_erase(macro_map_t **root, macro_compare_signature(const type *key, cmp_no_arg, type))

#define __macro_map_compare_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_map_compare_h(prefix##_find, macro_map_default(), type); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type); \
    MOD type * prefix##_erase(macro_map_t **root, macro_compare_signature(const type *key, cmp_no_arg, type))

#define __macro_map_compare_api(MOD, prefix, type) \
    MOD macro_map_find_compare(prefix##_find, type) \
    MOD macro_map_insert_compare(prefix##_insert, type) \
    MOD macro_map_remove(prefix##_remove, type) \
    MOD type * prefix##_erase(macro_map_t **root, macro_compare_signature(const type *key, cmp_no_arg, type)) { \
        type *node = prefix##_find(*root, key, cmp); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

#define __macro_map_compare_api_with_field(MOD, prefix, field, type) \
    MOD macro_map_find_compare_with_field(prefix##_find, field, type) \
    MOD macro_map_insert_compare_with_field(prefix##_insert, field, type) \
    MOD macro_map_remove_with_field(prefix##_remove, field, type) \
    MOD type * prefix##_erase(macro_map_t **root, macro_compare_signature(const type *key, cmp_no_arg, type)) { \
        type *node = prefix##_find(*root, key, cmp); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

/* KV Maps Compare */
#define __macro_map_kv_compare_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_map_kv_compare_h(prefix##_find, macro_map_default(), key_type, value_type); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type); \
    MOD value_type * prefix##_erase(macro_map_t **root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type))

#define __macro_map_kv_compare_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_map_kv_compare_h(prefix##_find, macro_map_default(), key_type, value_type); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type); \
    MOD value_type * prefix##_erase(macro_map_t **root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type))

#define __macro_map_kv_compare_api(MOD, prefix, key_type, value_type) \
    MOD macro_map_find_kv_compare(prefix##_find, key_type, value_type) \
    MOD macro_map_insert_compare(prefix##_insert, value_type) \
    MOD macro_map_remove(prefix##_remove, value_type) \
    MOD value_type * prefix##_erase(macro_map_t **root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type)) { \
        value_type *node = prefix##_find(*root, key, cmp); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

#define __macro_map_kv_compare_api_with_field(MOD, prefix, field, key_type, value_type) \
    MOD macro_map_find_kv_compare_with_field(prefix##_find, field, key_type, value_type) \
    MOD macro_map_insert_compare_with_field(prefix##_insert, field, value_type) \
    MOD macro_map_remove_with_field(prefix##_remove, field, value_type) \
    MOD value_type * prefix##_erase(macro_map_t **root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type)) { \
        value_type *node = prefix##_find(*root, key, cmp); \
        if (node) prefix##_remove(root, node); \
        return node; \
    }

/* Multimaps Compare */
#define __macro_multimap_compare_api_h(MOD, prefix, type) \
    MOD _macro_map_compare_h(prefix##_lower_bound, macro_map_default(), type); \
    MOD type * prefix##_find(const macro_map_t *root, macro_compare_signature(const type *key, cmp_no_arg, type)); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type)

#define __macro_multimap_compare_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_map_compare_h(prefix##_lower_bound, macro_map_default(), type); \
    MOD type * prefix##_find(const macro_map_t *root, macro_compare_signature(const type *key, cmp_no_arg, type)); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), type); \
    MOD macro_map_remove_h(prefix##_remove, type)

#define __macro_multimap_compare_api(MOD, prefix, type) \
    MOD _macro_map_compare(prefix##_lower_bound, lower_bound, macro_map_default(), type) \
    MOD type * prefix##_find(const macro_map_t *root, macro_compare_signature(const type *key, cmp_no_arg, type)) { \
        type *node = prefix##_lower_bound(root, key, cmp); \
        if (node && macro_equal_cmp_no_arg(type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert_compare(prefix##_insert, type) \
    MOD macro_map_remove(prefix##_remove, type)

#define __macro_multimap_compare_api_with_field(MOD, prefix, field, type) \
    MOD _macro_map_compare_with_field(prefix##_lower_bound, field, lower_bound, macro_map_default(), type) \
    MOD type * prefix##_find(const macro_map_t *root, macro_compare_signature(const type *key, cmp_no_arg, type)) { \
        type *node = prefix##_lower_bound(root, key, cmp); \
        if (node && macro_equal_cmp_no_arg(type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert_compare_with_field(prefix##_insert, field, type) \
    MOD macro_map_remove_with_field(prefix##_remove, field, type)

/* KV Multimaps Compare */
#define __macro_multimap_kv_compare_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_map_kv_compare_h(prefix##_lower_bound, macro_map_default(), key_type, value_type); \
    MOD value_type * prefix##_find(const macro_map_t *root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type)); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type)

#define __macro_multimap_kv_compare_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_map_kv_compare_h(prefix##_lower_bound, macro_map_default(), key_type, value_type); \
    MOD value_type * prefix##_find(const macro_map_t *root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type)); \
    MOD _macro_map_insert_compare_h(prefix##_insert, macro_map_default(), value_type); \
    MOD macro_map_remove_h(prefix##_remove, value_type)

#define __macro_multimap_kv_compare_api(MOD, prefix, key_type, value_type) \
    MOD _macro_map_kv_compare(prefix##_lower_bound, lower_bound, macro_map_default(), key_type, value_type) \
    MOD value_type * prefix##_find(const macro_map_t *root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type)) { \
        value_type *node = prefix##_lower_bound(root, key, cmp); \
        if (node && macro_cmp_kv_cmp_no_arg(key_type, value_type, cmp, key, node) == 0) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert_compare(prefix##_insert, value_type) \
    MOD macro_map_remove(prefix##_remove, value_type)

#define __macro_multimap_kv_compare_api_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_map_kv_compare_with_field(prefix##_lower_bound, field, lower_bound, macro_map_default(), key_type, value_type) \
    MOD value_type * prefix##_find(const macro_map_t *root, macro_compare_kv_signature(const key_type *key, cmp_no_arg, key_type, value_type)) { \
        value_type *node = prefix##_lower_bound(root, key, cmp); \
        if (node && macro_cmp_kv_cmp_no_arg(key_type, value_type, cmp, key, node) == 0) return node; \
        return NULL; \
    } \
    MOD macro_multimap_insert_compare_with_field(prefix##_insert, field, value_type) \
    MOD macro_map_remove_with_field(prefix##_remove, field, value_type)

/* ==========================================================================
 * PUBLIC GENERATORS
 * ========================================================================== */

/* Standard Map APIs */
#define macro_map_api_h(prefix, type)                            __macro_map_api_h(, prefix, type)
#define macro_map_static_api_h(prefix, type)                     __macro_map_api_h(static, prefix, type)
#define macro_map_static_inline_api_h(prefix, type)              __macro_map_api_h(static inline, prefix, type)

#define macro_map_api_h_with_field(prefix, field, type)               __macro_map_api_h_with_field(, prefix, field, type)
#define macro_map_static_api_h_with_field(prefix, field, type)        __macro_map_api_h_with_field(static, prefix, field, type)
#define macro_map_static_inline_api_h_with_field(prefix, field, type) __macro_map_api_h_with_field(static inline, prefix, field, type)

#define macro_map_api(prefix, type, cmp)                         __macro_map_api(, prefix, type, cmp)
#define macro_map_static_api(prefix, type, cmp)                  __macro_map_api(static, prefix, type, cmp)
#define macro_map_static_inline_api(prefix, type, cmp)           __macro_map_api(static inline, prefix, type, cmp)

#define macro_map_api_with_field(prefix, field, type, cmp)               __macro_map_api_with_field(, prefix, field, type, cmp)
#define macro_map_static_api_with_field(prefix, field, type, cmp)        __macro_map_api_with_field(static, prefix, field, type, cmp)
#define macro_map_static_inline_api_with_field(prefix, field, type, cmp) __macro_map_api_with_field(static inline, prefix, field, type, cmp)

/* KV Map APIs */
#define macro_map_kv_api_h(prefix, key_t, val_t)                 __macro_map_kv_api_h(, prefix, key_t, val_t)
#define macro_map_static_kv_api_h(prefix, key_t, val_t)          __macro_map_kv_api_h(static, prefix, key_t, val_t)
#define macro_map_static_inline_kv_api_h(prefix, key_t, val_t)   __macro_map_kv_api_h(static inline, prefix, key_t, val_t)

#define macro_map_kv_api_h_with_field(prefix, f, key_t, val_t)               __macro_map_kv_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_map_static_kv_api_h_with_field(prefix, f, key_t, val_t)        __macro_map_kv_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_map_static_inline_kv_api_h_with_field(prefix, f, key_t, val_t) __macro_map_kv_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_map_kv_api(prefix, key_t, val_t, cmp)              __macro_map_kv_api(, prefix, key_t, val_t, cmp)
#define macro_map_static_kv_api(prefix, key_t, val_t, cmp)       __macro_map_kv_api(static, prefix, key_t, val_t, cmp)
#define macro_map_static_inline_kv_api(prefix, key_t, val_t, cmp) __macro_map_kv_api(static inline, prefix, key_t, val_t, cmp)

#define macro_map_kv_api_with_field(prefix, f, key_t, val_t, cmp)               __macro_map_kv_api_with_field(, prefix, f, key_t, val_t, cmp)
#define macro_map_static_kv_api_with_field(prefix, f, key_t, val_t, cmp)        __macro_map_kv_api_with_field(static, prefix, f, key_t, val_t, cmp)
#define macro_map_static_inline_kv_api_with_field(prefix, f, key_t, val_t, cmp) __macro_map_kv_api_with_field(static inline, prefix, f, key_t, val_t, cmp)

/* Multimap APIs */
#define macro_multimap_api_h(prefix, type)                            __macro_multimap_api_h(, prefix, type)
#define macro_multimap_static_api_h(prefix, type)                     __macro_multimap_api_h(static, prefix, type)
#define macro_multimap_static_inline_api_h(prefix, type)              __macro_multimap_api_h(static inline, prefix, type)

#define macro_multimap_api_h_with_field(prefix, field, type)               __macro_multimap_api_h_with_field(, prefix, field, type)
#define macro_multimap_static_api_h_with_field(prefix, field, type)        __macro_multimap_api_h_with_field(static, prefix, field, type)
#define macro_multimap_static_inline_api_h_with_field(prefix, field, type) __macro_multimap_api_h_with_field(static inline, prefix, field, type)

#define macro_multimap_api(prefix, type, cmp)                         __macro_multimap_api(, prefix, type, cmp)
#define macro_multimap_static_api(prefix, type, cmp)                  __macro_multimap_api(static, prefix, type, cmp)
#define macro_multimap_static_inline_api(prefix, type, cmp)           __macro_multimap_api(static inline, prefix, type, cmp)

#define macro_multimap_api_with_field(prefix, field, type, cmp)               __macro_multimap_api_with_field(, prefix, field, type, cmp)
#define macro_multimap_static_api_with_field(prefix, field, type, cmp)        __macro_multimap_api_with_field(static, prefix, field, type, cmp)
#define macro_multimap_static_inline_api_with_field(prefix, field, type, cmp) __macro_multimap_api_with_field(static inline, prefix, field, type, cmp)

/* KV Multimap APIs */
#define macro_multimap_kv_api_h(prefix, key_t, val_t)                 __macro_multimap_kv_api_h(, prefix, key_t, val_t)
#define macro_multimap_static_kv_api_h(prefix, key_t, val_t)          __macro_multimap_kv_api_h(static, prefix, key_t, val_t)
#define macro_multimap_static_inline_kv_api_h(prefix, key_t, val_t)   __macro_multimap_kv_api_h(static inline, prefix, key_t, val_t)

#define macro_multimap_kv_api_h_with_field(prefix, f, key_t, val_t)               __macro_multimap_kv_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_multimap_static_kv_api_h_with_field(prefix, f, key_t, val_t)        __macro_multimap_kv_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_multimap_static_inline_kv_api_h_with_field(prefix, f, key_t, val_t) __macro_multimap_kv_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_multimap_kv_api(prefix, key_t, val_t, cmp)              __macro_multimap_kv_api(, prefix, key_t, val_t, cmp)
#define macro_multimap_static_kv_api(prefix, key_t, val_t, cmp)       __macro_multimap_kv_api(static, prefix, key_t, val_t, cmp)
#define macro_multimap_static_inline_kv_api(prefix, key_t, val_t, cmp) __macro_multimap_kv_api(static inline, prefix, key_t, val_t, cmp)

#define macro_multimap_kv_api_with_field(prefix, f, key_t, val_t, cmp)               __macro_multimap_kv_api_with_field(, prefix, f, key_t, val_t, cmp)
#define macro_multimap_static_kv_api_with_field(prefix, f, key_t, val_t, cmp)        __macro_multimap_kv_api_with_field(static, prefix, f, key_t, val_t, cmp)
#define macro_multimap_static_inline_kv_api_with_field(prefix, f, key_t, val_t, cmp) __macro_multimap_kv_api_with_field(static inline, prefix, f, key_t, val_t, cmp)

/* ==========================================================================
 * PUBLIC COMPARE GENERATORS
 * ========================================================================== */

/* Standard Map Compare APIs */
#define macro_map_compare_api_h(prefix, type)                            __macro_map_compare_api_h(, prefix, type)
#define macro_map_static_compare_api_h(prefix, type)                     __macro_map_compare_api_h(static, prefix, type)
#define macro_map_static_inline_compare_api_h(prefix, type)              __macro_map_compare_api_h(static inline, prefix, type)

#define macro_map_compare_api_h_with_field(prefix, field, type)               __macro_map_compare_api_h_with_field(, prefix, field, type)
#define macro_map_static_compare_api_h_with_field(prefix, field, type)        __macro_map_compare_api_h_with_field(static, prefix, field, type)
#define macro_map_static_inline_compare_api_h_with_field(prefix, field, type) __macro_map_compare_api_h_with_field(static inline, prefix, field, type)

#define macro_map_compare_api(prefix, type)                         __macro_map_compare_api(, prefix, type)
#define macro_map_static_compare_api(prefix, type)                  __macro_map_compare_api(static, prefix, type)
#define macro_map_static_inline_compare_api(prefix, type)           __macro_map_compare_api(static inline, prefix, type)

#define macro_map_compare_api_with_field(prefix, field, type)               __macro_map_compare_api_with_field(, prefix, field, type)
#define macro_map_static_compare_api_with_field(prefix, field, type)        __macro_map_compare_api_with_field(static, prefix, field, type)
#define macro_map_static_inline_compare_api_with_field(prefix, field, type) __macro_map_compare_api_with_field(static inline, prefix, field, type)

/* KV Map Compare APIs */
#define macro_map_kv_compare_api_h(prefix, key_t, val_t)                 __macro_map_kv_compare_api_h(, prefix, key_t, val_t)
#define macro_map_static_kv_compare_api_h(prefix, key_t, val_t)          __macro_map_kv_compare_api_h(static, prefix, key_t, val_t)
#define macro_map_static_inline_kv_compare_api_h(prefix, key_t, val_t)   __macro_map_kv_compare_api_h(static inline, prefix, key_t, val_t)

#define macro_map_kv_compare_api_h_with_field(prefix, f, key_t, val_t)               __macro_map_kv_compare_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_map_static_kv_compare_api_h_with_field(prefix, f, key_t, val_t)        __macro_map_kv_compare_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_map_static_inline_kv_compare_api_h_with_field(prefix, f, key_t, val_t) __macro_map_kv_compare_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_map_kv_compare_api(prefix, key_t, val_t)              __macro_map_kv_compare_api(, prefix, key_t, val_t)
#define macro_map_static_kv_compare_api(prefix, key_t, val_t)       __macro_map_kv_compare_api(static, prefix, key_t, val_t)
#define macro_map_static_inline_kv_compare_api(prefix, key_t, val_t) __macro_map_kv_compare_api(static inline, prefix, key_t, val_t)

#define macro_map_kv_compare_api_with_field(prefix, f, key_t, val_t)               __macro_map_kv_compare_api_with_field(, prefix, f, key_t, val_t)
#define macro_map_static_kv_compare_api_with_field(prefix, f, key_t, val_t)        __macro_map_kv_compare_api_with_field(static, prefix, f, key_t, val_t)
#define macro_map_static_inline_kv_compare_api_with_field(prefix, f, key_t, val_t) __macro_map_kv_compare_api_with_field(static inline, prefix, f, key_t, val_t)

/* Multimap Compare APIs */
#define macro_multimap_compare_api_h(prefix, type)                            __macro_multimap_compare_api_h(, prefix, type)
#define macro_multimap_static_compare_api_h(prefix, type)                     __macro_multimap_compare_api_h(static, prefix, type)
#define macro_multimap_static_inline_compare_api_h(prefix, type)              __macro_multimap_compare_api_h(static inline, prefix, type)

#define macro_multimap_compare_api_h_with_field(prefix, field, type)               __macro_multimap_compare_api_h_with_field(, prefix, field, type)
#define macro_multimap_static_compare_api_h_with_field(prefix, field, type)        __macro_multimap_compare_api_h_with_field(static, prefix, field, type)
#define macro_multimap_static_inline_compare_api_h_with_field(prefix, field, type) __macro_multimap_compare_api_h_with_field(static inline, prefix, field, type)

#define macro_multimap_compare_api(prefix, type)                         __macro_multimap_compare_api(, prefix, type)
#define macro_multimap_static_compare_api(prefix, type)                  __macro_multimap_compare_api(static, prefix, type)
#define macro_multimap_static_inline_compare_api(prefix, type)           __macro_multimap_compare_api(static inline, prefix, type)

#define macro_multimap_compare_api_with_field(prefix, field, type)               __macro_multimap_compare_api_with_field(, prefix, field, type)
#define macro_multimap_static_compare_api_with_field(prefix, field, type)        __macro_multimap_compare_api_with_field(static, prefix, field, type)
#define macro_multimap_static_inline_compare_api_with_field(prefix, field, type) __macro_multimap_compare_api_with_field(static inline, prefix, field, type)

/* KV Multimap Compare APIs */
#define macro_multimap_kv_compare_api_h(prefix, key_t, val_t)                 __macro_multimap_kv_compare_api_h(, prefix, key_t, val_t)
#define macro_multimap_static_kv_compare_api_h(prefix, key_t, val_t)          __macro_multimap_kv_compare_api_h(static, prefix, key_t, val_t)
#define macro_multimap_static_inline_kv_compare_api_h(prefix, key_t, val_t)   __macro_multimap_kv_compare_api_h(static inline, prefix, key_t, val_t)

#define macro_multimap_kv_compare_api_h_with_field(prefix, f, key_t, val_t)               __macro_multimap_kv_compare_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_multimap_static_kv_compare_api_h_with_field(prefix, f, key_t, val_t)        __macro_multimap_kv_compare_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_multimap_static_inline_kv_compare_api_h_with_field(prefix, f, key_t, val_t) __macro_multimap_kv_compare_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_multimap_kv_compare_api(prefix, key_t, val_t)              __macro_multimap_kv_compare_api(, prefix, key_t, val_t)
#define macro_multimap_static_kv_compare_api(prefix, key_t, val_t)       __macro_multimap_kv_compare_api(static, prefix, key_t, val_t)
#define macro_multimap_static_inline_kv_compare_api(prefix, key_t, val_t) __macro_multimap_kv_compare_api(static inline, prefix, key_t, val_t)

#define macro_multimap_kv_compare_api_with_field(prefix, f, key_t, val_t)               __macro_multimap_kv_compare_api_with_field(, prefix, f, key_t, val_t)
#define macro_multimap_static_kv_compare_api_with_field(prefix, f, key_t, val_t)        __macro_multimap_kv_compare_api_with_field(static, prefix, f, key_t, val_t)
#define macro_multimap_static_inline_kv_compare_api_with_field(prefix, f, key_t, val_t) __macro_multimap_kv_compare_api_with_field(static inline, prefix, f, key_t, val_t)

/* multimap or iterator removal important notes!!!

// Because of how intrusive structures work, dropping erase and forcing the caller
// to use remove introduces one classic C trap you should be aware of: you must cache
// the next node before removing the current one.
//
// When macro_map_erase unlinks a node, it scrambles that node's left, right, and
// parent_color pointers. If you try to call macro_map_next() on a node after you remove
// it, it will segfault or corrupt the tree.
//
// Here is the safe, idiomatic way a user will consume your generated multimap API
// to delete all duplicates:

// Generate the multimap interface
macro_multimap_kv_api(alias_index, char, alias_node_t, cmp_alias)

// Safe removal loop
void remove_all_aliases(macro_map_t **root, const char *search_key) {
    // 1. Get the FIRST exact match (returns NULL if "search_key" isn't found)
    alias_node_t *curr = alias_index_find(*root, search_key);

    while (curr && cmp_alias(search_key, curr) == 0) {
        // 2. CACHE the next node BEFORE removing!
        macro_map_t *next_link = macro_map_next(&curr->node);
        alias_node_t *next = next_link ? (alias_node_t *)next_link : NULL; // or macro_parent_object

        // 3. Remove and free the current node safely
        alias_index_remove(root, curr);
        free(curr); // or aml_pool_free(...)

        // 4. Advance
        curr = next;
    }
}
*/

#endif
