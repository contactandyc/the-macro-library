#ifndef _macro_string_id_map_H
#define _macro_string_id_map_H

/* This is a very commonly used map type.  The string is expected to be a zero terminated c style string just after
   the macro_string_id_map_t structure.  Typically, reserve the id zero for NULL and not found.  Sometimes, it makes
   sense to also insert an empty string as 0 to make NULL, empty string, and not found the same.

   This also can serve as an example for building a new map type.

   The caller is expected to allocate the macro_string_id_map_t structure prior to insert.

   #include "a-memory-library/aml_pool.h" // other allocators can be used
   #include <stdio.h>
   #include <string.h>

   aml_pool_t *pool = aml_pool_init(16384);
   macro_map_t *root = NULL;

   void macro_string_id_insert(aml_pool_t *pool, macro_map_t **root, const char *key, size_t value) {
       // assuming that the str/id doesn't already exist!
       macro_string_id_map_t* node = (macro_string_id_map_t*)aml_pool_alloc(pool, sizeof(*node) + strlen(key)+1);
       strcpy((char *)(node+1), str);
       node->id = value;
       _macro_string_id_insert(root, node); // notice the underscore!
    }

    macro_string_id_insert(pool, &root, "Hello", 1);
    macro_string_id_insert(pool, &root, "World", 2);
    printf( "World == %zu\n", macro_string_id_find(root, "World")); // should print 2
*/

#include "the-macro-library/macro_map.h"

#include <string.h>

typedef struct {
    macro_map_t node;
    size_t id;
} macro_string_id_map_t;

static inline
int compare_macro_string_id_map_for_insert(const macro_string_id_map_t *p1, const macro_string_id_map_t *p2) {
    const char *a = (const char *)(p1+1);
    const char *b = (const char *)(p2+1);
    return strcmp(a, b);
}

static inline
int compare_macro_string_id_map_for_find(const char *a, const macro_string_id_map_t *p2) {
    const char *b = (const char *)(p2+1);
    return strcmp(a, b);
}

static macro_map_insert(_macro_string_id_insert, macro_string_id_map_t, compare_macro_string_id_map_for_insert);
// void _macro_string_id_insert(macro_map_t **root, macro_string_id_map_t *item);
static macro_map_find_kv(_macro_string_id_find, char,  macro_string_id_map_t, compare_macro_string_id_map_for_find);
// macro_string_id_map_t* _macro_string_id_find(const macro_map_t *root, const char *str);

static inline size_t macro_string_id_find(const macro_map_t *root, const char *str) {
    if(!str)
        return 0;

    macro_string_id_map_t* r = _macro_string_id_find(root, str);
    if(r) return r->id;
    return 0;
}

#endif