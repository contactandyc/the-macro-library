// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_lru_H
#define _macro_lru_H

/* Intrusive Doubly-Linked List macros for LRU tracking. */

#define macro_dlist_remove(head, tail, node) do { \
    if ((node)->prev) (node)->prev->next = (node)->next; \
    else (head) = (node)->next; \
    if ((node)->next) (node)->next->prev = (node)->prev; \
    else (tail) = (node)->prev; \
} while(0)

#define macro_dlist_push_front(head, tail, node) do { \
    (node)->next = (head); \
    (node)->prev = NULL; \
    if ((head)) (head)->prev = (node); \
    (head) = (node); \
    if (!(tail)) (tail) = (node); \
} while(0)

#define macro_dlist_pop_back(head, tail, out_node) do { \
    (out_node) = (tail); \
    if (out_node) { \
        macro_dlist_remove(head, tail, out_node); \
        (out_node)->next = (out_node)->prev = NULL; \
    } \
} while(0)

#define macro_lru_promote(head, tail, node) do { \
    if ((head) != (node)) { \
        macro_dlist_remove(head, tail, node); \
        macro_dlist_push_front(head, tail, node); \
    } \
} while(0)

#endif /* _MACRO_LRU_H */
