#ifndef PLAYGROUND_HASHTABLE_H
#define PLAYGROUND_HASHTABLE_H

#include "common/macros.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#define Pair(TKey, TValue)      TKey##TValue##Pair

#define DeclarePair(TKey, TValue) \
typedef struct { \
    TKey Key; \
    TValue Value; \
} Pair(TKey, TValue)


typedef uint64_t (*HashFunction)(const void *);

typedef int (*HashTableComparator)(const void *, const void *, size_t n);

typedef void (*HashTableEntryDestructor)(void *);


struct HashTableHeader;


void *HashTable_New(
        HashFunction hash,
        HashTableComparator compare,
        size_t buckets_count,
        size_t key_size,
        size_t key_offset,
        size_t value_size,
        size_t value_offset,
        size_t entry_size
);


#define HASHTABLE_DEFAULT_BUCKETS_COUNT 128
#define HASHTABLE_MIN_BUCKETS_COUNT     1

#define HashTable(entry_type, hash_fn, compare_fn) \
(entry_type **) HashTable_New( \
    hash_fn, compare_fn, \
    HASHTABLE_DEFAULT_BUCKETS_COUNT, \
    sizeof_member(entry_type, Key), offsetof(entry_type, Key), \
    sizeof_member(entry_type, Value), offsetof(entry_type, Value), \
    sizeof(entry_type) \
)

void *HashTable_PutEntries(
        void *table,
        const void *entries,
        size_t entries_count
);

#define HashTable_PutAll(table, ...) \
(typeof(**table) **) HashTable_PutEntries( \
    (table), \
    array_literal_of_type_sized(typeof(**table), __VA_ARGS__) \
)

void *HashTable_AtKey(void *table, const void *key);

#define HashTable_At(table, key) \
(typeof_member(typeof(**table), Value) *) HashTable_AtKey(\
    (void **) (table), \
    array_literal_of_type(typeof_member(typeof(**table), Key), key)\
)

void *HashTable_PutEntry(void *table, const void *entry);

#define HashTable_Put(table, key, value) \
(typeof_member(typeof(**table), Value) *) HashTable_PutEntry( \
    (void**) (table), \
    &((typeof(**table)) {key, value}) \
)

void HashTable_PrintStats(void *table);

void HashTable_Free(void *table, HashTableEntryDestructor free_entry);

typedef struct {
    const struct HashTableHeader *const _table;
    size_t BucketIndex;
    size_t EntryIndex;
} HashTableIterator;

size_t HashTable_FindNextUsedBucket(size_t current_bucket_index, void **buckets);

HashTableIterator HashTable_BeginEntries(void *table);

bool HashTable_HasMoreEntries(HashTableIterator it);

void HashTable_NextEntry(HashTableIterator *it);

#define HashTable_Entry(table, iterator)    (table)[(iterator).BucketIndex][(iterator).EntryIndex]

#define HashTable_ForEach(entry_name, table)                                \
HashTableIterator CONCAT(_it_, __LINE__) = HashTable_BeginEntries(table);   \
for (                                                                       \
    typeof(**table) entry_name = (                                          \
        HashTable_HasMoreEntries(CONCAT(_it_, __LINE__))                    \
            ? HashTable_Entry(table, CONCAT(_it_, __LINE__))                \
            : (typeof(**table)) {}                                          \
    );                                                                      \
    HashTable_HasMoreEntries(CONCAT(_it_, __LINE__));                       \
    HashTable_NextEntry(&CONCAT(_it_, __LINE__)),                           \
        (HashTable_HasMoreEntries(CONCAT(_it_, __LINE__))                   \
            ? (entry_name = HashTable_Entry(table, CONCAT(_it_, __LINE__))) \
            : entry_name)                                                   \
)

#endif //PLAYGROUND_HASHTABLE_H
