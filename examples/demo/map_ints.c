#include "the-macro-library/macro_map.h"

typedef struct {
    macro_map_t node;
    int value;
} int_node;

static inline
int compare_int(const int_node *a, const int_node *b) {
    return a->value - b->value;
}

macro_multimap_insert(insert_int, int_node, compare_int);

macro_map_find(find_int, int_node, compare_int);
_macro_map(first_int, first, cmp_no_arg, int_node, compare_int);
_macro_map(last_int, last, cmp_no_arg, int_node, compare_int);
_macro_map(floor_int, floor, cmp_no_arg, int_node, compare_int);
_macro_map(ceiling_int, ceiling, cmp_no_arg, int_node, compare_int);
_macro_map(lower_bound_int, lower_bound, cmp_no_arg, int_node, compare_int);
_macro_map(upper_bound_int, upper_bound, cmp_no_arg, int_node, compare_int);

int main() {
    int_node nodes[4];
    macro_map_t *root = NULL;
    nodes[0].value = 3;
    insert_int(&root, nodes+0);
    nodes[1].value = 1;
    insert_int(&root, nodes+1);
    nodes[2].value = 5;
    insert_int(&root, nodes+2);
    nodes[3].value = 3;
    insert_int(&root, nodes+3);

    macro_map_t *n = macro_map_first(root);
    while(n) {
        int_node *inode = (int_node *)n;
        printf( "%d, %ld\n", inode->value, inode-nodes );
        n = macro_map_next(n);
    }

    for( int i=-1; i<8; i++ ) {
        printf( "\n==============\nkey: %d\n\n", i );
        int_node key;
        key.value = i;
        int_node *inode = first_int(root, &key);
        if(inode)
            printf( "first %d, %ld\n", inode->value, inode-nodes );

        inode = last_int(root, &key);
        if(inode)
            printf( "last %d, %ld\n", inode->value, inode-nodes );

        inode = floor_int(root, &key);
        if(inode)
            printf( "floor %d, %ld\n", inode->value, inode-nodes );

        inode = ceiling_int(root, &key);
        if(inode)
            printf( "ceiling %d, %ld\n", inode->value, inode-nodes );

        inode = lower_bound_int(root, &key);
        if(inode)
            printf( "lower_bound %d, %ld\n", inode->value, inode-nodes );

        inode = upper_bound_int(root, &key);
        if(inode)
            printf( "upper_bound %d, %ld\n", inode->value, inode-nodes );
    }
    return 0;
}