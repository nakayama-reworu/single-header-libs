#ifndef PLAYGROUND_HASHTABLE_H
#define PLAYGROUND_HASHTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

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
    SIZEOF_MEMBER(entry_type, Key), offsetof(entry_type, Key), \
    SIZEOF_MEMBER(entry_type, Value), offsetof(entry_type, Value), \
    sizeof(entry_type) \
)

void *HashTable_AtKey(void *table, const void *key);

#define HashTable_At(table, key) \
(TYPEOF_MEMBER(typeof(**table), Value) *) HashTable_AtKey(\
    (void **) (table), \
    ARRAY_LITERAL_FROM_VA_ARGS((TYPEOF_MEMBER(typeof(**table), Key)) key)\
)

void *HashTable_PutEntry(void *table, const void *entry);

#define HashTable_Put(table, key, value) \
(TYPEOF_MEMBER(typeof(**table), Value) *) HashTable_PutEntry( \
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

#endif //PLAYGROUND_HASHTABLE_H
