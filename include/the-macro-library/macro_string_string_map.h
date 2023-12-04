#ifndef _macro_string_string_map_H
#define _macro_string_string_map_H

/* This is a very commonly used map type.  The strings are expected to be a zero terminated c style string just after
   the macro_map_t structure.

   This also can serve as an example for building a new map type.

   The caller is expected to allocate the macro_map_t structure prior to insert.

   #include "a-memory-library/aml_pool.h" // other allocators can be used
   #include <stdio.h>
   #include <string.h>

   aml_pool_t *pool = aml_pool_init(16384);
   macro_map_t *root = NULL;

   void macro_string_string_insert(aml_pool_t *pool, macro_map_t **root, const char *key, const char *value) {
       // assuming that the str/id doesn't already exist!
       macro_map_t* node = (macro_map_t*)aml_pool_alloc(pool, sizeof(*node) + strlen(key)+strlen(value)+2);
       strcpy((char *)(node+1), key);
       strcpy(key+strlen(key)+1, value);
       _macro_string_string_insert(root, node); // notice the underscore!
    }

    macro_string_string_insert(pool, &root, "Hello", "World");
    macro_string_string_insert(pool, &root, "Good", "Bye);
    printf( "World == %s\n", macro_string_id_find(root, "Good")); // should print Bye
*/

#include "the-macro-library/macro_map.h"

#include <string.h>

static inline
int compare_macro_string_string_map_for_insert(const macro_map_t *p1, const macro_map_t *p2) {
    const char *a = (const char *)(p1+1);
    const char *b = (const char *)(p2+1);
    return strcmp(a, b);
}

static inline
int compare_macro_string_string_map_for_find(const char *a, const macro_map_t *p2) {
    const char *b = (const char *)(p2+1);
    return strcmp(a, b);
}

static macro_map_insert(_macro_string_string_insert, macro_map_t, compare_macro_string_string_map_for_insert);
// void _macro_string_string_insert(macro_map_t **root, macro_map_t *item);
static macro_map_find_kv(_macro_string_string_find, char,  macro_map_t, compare_macro_string_string_map_for_find);
// macro_map_t* _macro_string_string_find(const macro_map_t *root, const char *key);

static inline const char *macro_string_string_find(const macro_map_t *root, const char *key) {
    if(!key)
        return NULL;

    macro_map_t* r = _macro_string_string_find(root, key);
    if(r) {
        const char *k = (const char *)(r+1);
        return k+strlen(k)+1; // value is after key
    }

    return NULL;
}

#endif