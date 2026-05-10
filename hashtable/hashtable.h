#ifndef PLAYGROUND_HASHTABLE_H
#define PLAYGROUND_HASHTABLE_H

#include "common/macros.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdalign.h>

#define Pair(TKey, TValue)      TKey##TValue##Pair

#define DeclarePair(TKey, TValue) \
typedef struct { \
    TKey Key; \
    TValue Value; \
} Pair(TKey, TValue)


typedef size_t (*HashFunction)(const void *p_key);

typedef int (*HashTableComparator)(const void *p_key1, const void *p_key2);

typedef void (*HashTableEntryDestructor)(void *p_entry);


void *hashtable_new(
        HashFunction hash,
        HashTableComparator compare,
        size_t slots_count,
        size_t key_size,
        size_t key_offset,
        size_t value_size,
        size_t value_offset,
        size_t record_size,
        size_t record_alignment
);

#define DEFAULT_SLOTS_COUNT     (64)

#define hashtable(entry_type, hash, compare)                        \
(entry_type *) hashtable_new(                                       \
    (hash), (compare),                                              \
    DEFAULT_SLOTS_COUNT,                                            \
    sizeof_member(entry_type, Key), offsetof(entry_type, Key),      \
    sizeof_member(entry_type, Value), offsetof(entry_type, Value),  \
    sizeof(entry_type), alignof(entry_type)                         \
)

void hashtable_free(void *, HashTableEntryDestructor);

void *hashtable_value_at(const void *, const void *key);

#define hashtable_at(table, key)                                        \
(typeof_member(typeof(*(table)), Value) *) hashtable_value_at(          \
    (table),                                                            \
    array_literal_of_type(typeof_member(typeof(*(table)), Key), (key))  \
)

/*
#define hashtable_get(table, key)                           \
({                                                          \
    __auto_type *_p_value = hashtable_at((table), (key));   \
    assert(NULL != _p_value);                               \
    *_p_value;                                              \
})
 */

void *hashtable_put_entry(void *, const void *entry);

#define hashtable_put(table, key, value) \
((table) = hashtable_put_entry((table), &((typeof(*(table))) {.Key=(key), .Value=(value)})))

size_t hashtable_size(const void *);

/*
#define hashtable_empty(table)  (0 == hashtable_size(table))
 */

typedef struct {
    size_t _i;
} HashTableIterator;

HashTableIterator hashtable_begin(const void *);

bool hashtable_end(const void *, HashTableIterator);

void hashtable_next(const void *, HashTableIterator *);

const void *hashtable_get_entry(const void *, HashTableIterator);

void hashtable_copy_entry(const void *, HashTableIterator, void *dst);

#define hashtable_foreach(entry_name, table)                                            \
HashTableIterator CONCAT(_i_, __LINE__) = hashtable_begin(table);                                  \
for (                                                                                   \
    __auto_type entry_name =                                         \
        (hashtable_end((table), CONCAT(_i_, __LINE__))                                \
            ? (typeof(*table)) {}                                                                  \
            : *(typeof(*table) *) hashtable_get_entry(                              \
                (table),                                                                \
                CONCAT(_i_, __LINE__)                                                          \
            )  \
        );                                                                      \
    !hashtable_end((table), CONCAT(_i_, __LINE__));                                     \
    hashtable_next((table), &CONCAT(_i_, __LINE__)),             \
        hashtable_copy_entry((table), CONCAT(_i_, __LINE__), &entry_name)                  \
)


#endif // PLAYGROUND_HASHTABLE_H
