#include "hashtable.h"

#include "common/macros.h"
#include "dynarray/dynarray.h"


typedef struct HashTableHeader {
    void **Buckets;

    HashFunction Hash;
    HashTableComparator Compare;

    size_t BucketsCount;

    size_t KeySize;
    size_t KeyOffset;
    size_t ValueSize;
    size_t ValueOffset;
    size_t EntrySize;
} HashTableHeader;

#define IMPL_HASHTABLE_HEADER(table)    ((HashTableHeader *) (table) - 1)

void *HashTable_New(
        HashFunction hash,
        HashTableComparator compare,
        size_t buckets_count,
        size_t key_size,
        size_t key_offset,
        size_t value_size,
        size_t value_offset,
        size_t entry_size
) {
    if (buckets_count <= HASHTABLE_MIN_BUCKETS_COUNT) {
        LOG_ERROR("Buckets count must be at least %d", HASHTABLE_MIN_BUCKETS_COUNT);
        return NULL;
    }

    LOG_DEBUG(
            NAME_OF(buckets_count) "=%d, "
    NAME_OF(key_size) "=%d, " NAME_OF(key_offset) "=%d, "
    NAME_OF(value_size) "=%d, " NAME_OF(value_offset) "=%d",
            (int) buckets_count, (int) key_size, (int) key_offset, (int) value_size, (int) value_offset
    );
    const size_t total_bytes = buckets_count * sizeof(void *) + sizeof(HashTableHeader);

    HashTableHeader *header;
    if (NULL == (header = calloc(total_bytes, 1))) {
        perror(NAME_OF(HashTable_New));
        return NULL;
    }


    *header = (HashTableHeader) {
            .Buckets = SHIFT(header, sizeof(HashTableHeader)),
            .Hash = hash,
            .Compare = compare,
            .BucketsCount = buckets_count,
            .KeySize = key_size,
            .KeyOffset = key_offset,
            .ValueSize = value_size,
            .ValueOffset = value_offset,
            .EntrySize= entry_size,
    };

    return header->Buckets;
}


void *HashTable_AtKey(void *table, const void *key) {
    if (NULL == table) {
        LOG_NULL(buckets);
        return NULL;
    }

    void **buckets = table;
    HashTableHeader *header = IMPL_HASHTABLE_HEADER(table);

    const uint64_t hash = header->Hash(key);
    void *bucket = buckets[hash % header->BucketsCount];

    if (NULL == bucket) {
        return NULL;
    }

    for (size_t i = 0; i < Array_Size(bucket); i++) {
        void *entry = Array_At(bucket, i);
        if (0 == header->Compare(SHIFT(entry, header->KeyOffset), key, header->KeySize)) {
            return SHIFT(entry, header->ValueOffset);
        }
    }

    return NULL;
}


void *HashTable_PutEntry(void *table, const void *entry) {
    if (NULL == table) {
        LOG_NULL(table);
        return NULL;
    }

    void **buckets = table;
    HashTableHeader *header = IMPL_HASHTABLE_HEADER(buckets);

    const void *key = SHIFT(entry, header->KeyOffset);
    const void *value = SHIFT(entry, header->ValueOffset);

    void *existing_value;
    if (NULL != (existing_value = HashTable_AtKey(buckets, key))) {
        LOG_DEBUG("Found existing entry, overwriting value");
        return memcpy(existing_value, value, header->ValueSize);
    }

    LOG_DEBUG("Creating new entry");

    const size_t bucket_index = header->Hash(key) % header->BucketsCount;
    void *bucket = buckets[bucket_index];

    if (NULL == bucket) {
        LOG_DEBUG("Creating new bucket");

        if (NULL == (bucket = Array_New(0, header->EntrySize))) {
            LOG_ERROR("Failed to allocate a bucket");
            return NULL;
        }
    }

    buckets[bucket_index] = bucket = Array_AppendFrom(bucket, entry);
    return SHIFT(Array_At(bucket, Array_Size(bucket) - 1), header->ValueOffset);
}


void HashTable_PrintStats(void *table) {
    if (NULL == table) {
        LOG_NULL(table);
        return;
    }

    void **buckets = table;
    HashTableHeader *header = IMPL_HASHTABLE_HEADER(table);

    size_t used_buckets_count = 0;
    size_t total_entries = 0;

    int empty_begin = -1, empty_end = empty_begin;

    printf("Total buckets: %d\n", (int) header->BucketsCount);

    for (size_t i = 0; i < header->BucketsCount; i++) {
        if (NULL == buckets[i]) {
            if (-1 == empty_begin) {
                empty_begin = empty_end = (int) i;
            } else {
                empty_end = (int) i;
            }
        } else {
            if (empty_begin >= 0) {
                if (empty_begin == empty_end) {
                    printf(NAME_OF(buckets)"[%d] = NULL\n", empty_begin);
                } else {
                    printf(NAME_OF(buckets)"[%d..%d] = NULL\n", empty_begin, empty_end);
                }

                empty_begin = empty_end = -1;
            }
            used_buckets_count++;
            total_entries += Array_Size(buckets[i]);
            printf(NAME_OF(buckets)"[%d] = Array{.Size=%d}\n", (int) i, (int) Array_Size(buckets[i]));
        }
    }

    if (empty_begin >= 0) {
        if (empty_begin == empty_end) {
            printf(NAME_OF(buckets)"[%d] = NULL\n", empty_begin);
        } else {
            printf(NAME_OF(buckets)"[%d..%d] = NULL\n", empty_begin, empty_end);
        }
    }

    printf("Buckets used: %d\n", (int) used_buckets_count);
    printf("Total entries: %d\n", (int) total_entries);
}


void HashTable_Free(void *table, HashTableEntryDestructor free_entry) {
    if (NULL == table) {
        LOG_NULL(buckets);
        return;
    }
    void **buckets = table;
    HashTableHeader *header = IMPL_HASHTABLE_HEADER(buckets);

    for (size_t i = 0; i < header->BucketsCount; i++) {
        void *bucket = buckets[i];
        if (NULL == bucket) {
            continue;
        }

        if (NULL != free_entry) {
            Array_ForEachElement(bucket, free_entry);
        }
        Array_FreeAndSetToNull(bucket);
    }

    free(header);
}


size_t HashTable_FindNextUsedBucket(size_t current_bucket_index, void **buckets) {
    const HashTableHeader *table = IMPL_HASHTABLE_HEADER(buckets);
    for (; current_bucket_index < table->BucketsCount; current_bucket_index++) {
        if (NULL != buckets[current_bucket_index]) {
            break;
        }
    }

    return current_bucket_index;
}


HashTableIterator HashTable_BeginEntries(void *table) {
    return (HashTableIterator) {
            ._table = IMPL_HASHTABLE_HEADER(table),
            .BucketIndex = HashTable_FindNextUsedBucket(0, table),
            .EntryIndex = 0,
    };
}


bool HashTable_HasMoreEntries(const HashTableIterator it) {
    return it.BucketIndex < it._table->BucketsCount;
}


void HashTable_NextEntry(HashTableIterator *it) {
    void **buckets = it->_table->Buckets;
    if (it->EntryIndex + 1 < Array_Size(it->_table->Buckets[it->BucketIndex])) {
        it->EntryIndex++;
        return;
    }

    it->BucketIndex = HashTable_FindNextUsedBucket(it->BucketIndex + 1, buckets);
    it->EntryIndex = 0;
}

