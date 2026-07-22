// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#include <stddef.h>
#include "the-macro-library/macro_lru.h"
#include "the-macro-library/macro_test.h"

typedef struct test_node_s {
    int id;
    struct test_node_s *prev;
    struct test_node_s *next;
} test_node_t;

MACRO_TEST(lru_push_and_pop) {
    test_node_t *head = NULL, *tail = NULL;
    test_node_t n1 = {1, NULL, NULL};
    test_node_t n2 = {2, NULL, NULL};

    macro_dlist_push_front(head, tail, &n1);
    MACRO_ASSERT_TRUE(head == &n1 && tail == &n1);

    macro_dlist_push_front(head, tail, &n2);
    MACRO_ASSERT_TRUE(head == &n2 && tail == &n1);
    MACRO_ASSERT_TRUE(n2.next == &n1 && n1.prev == &n2);

    test_node_t *out;
    macro_dlist_pop_back(head, tail, out);
    MACRO_ASSERT_TRUE(out == &n1);
    MACRO_ASSERT_TRUE(head == &n2 && tail == &n2);
}

MACRO_TEST(lru_promotion) {
    test_node_t *head = NULL, *tail = NULL;
    test_node_t nodes[3] = {{1,0,0}, {2,0,0}, {3,0,0}};

    macro_dlist_push_front(head, tail, &nodes[0]);
    macro_dlist_push_front(head, tail, &nodes[1]);
    macro_dlist_push_front(head, tail, &nodes[2]);
    /* List: 3 -> 2 -> 1 */

    macro_lru_promote(head, tail, &nodes[0]);
    /* List: 1 -> 3 -> 2 */

    MACRO_ASSERT_TRUE(head == &nodes[0]);
    MACRO_ASSERT_TRUE(tail == &nodes[1]);
    MACRO_ASSERT_TRUE(nodes[0].next == &nodes[2]);
}

int main(void) {
    macro_test_case tests[64];
    size_t test_count = 0;

    MACRO_ADD(tests, lru_push_and_pop);
    MACRO_ADD(tests, lru_promotion);

    macro_run_all("the-macro-library/macro_lru", tests, test_count);
    return 0;
}
