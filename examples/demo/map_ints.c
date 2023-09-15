#include "macro_map.h"

typedef struct {
    macro_map_t node;
    int value;
} int_node;

static inline
int compare_int(const int_node *a, const int_node *b) {
    return a->value - b->value;
}

macro_map_find(find_int, int_node, compare_int);
macro_map_insert(insert_int, int_node, compare_int);

int main() {
    int_node nodes[4];
    macro_map_t *root = NULL;
    nodes[0].value = 3;
    insert_int(&root, nodes+0);
    nodes[1].value = 1;
    insert_int(&root, nodes+1);
    nodes[2].value = 5;
    insert_int(&root, nodes+2);
    nodes[3].value = 0;
    insert_int(&root, nodes+3);

    macro_map_t *n = macro_map_first(root);
    while(n) {
        int_node *inode = (int_node *)n;
        printf( "%d\n", inode->value );
        n = macro_map_next(n);
    }

    int_node key;
    key.value = 4;
    int_node *inode = find_int(root, &key);
    if(inode)
        printf( "found %d\n", inode->value );
    return 0;
}