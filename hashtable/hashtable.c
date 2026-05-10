#include "hashtable.h"

#include "common/macros.h"

#include <string.h>
#include <assert.h>

#define LOAD_FACTOR_THRESHOLD   (0.5f)
#define GROWTH_RATE             (2)


typedef struct HashTable {
    HashFunction Hash;
    HashTableComparator Compare;

    size_t TotalSlots;
    size_t UsedSlots;

    size_t KeySize;
    size_t KeyOffset;
    size_t ValueSize;
    size_t ValueOffset;

    union {
        size_t PresenceFlagOffset;
        size_t KeyValueEntrySize;
    };

    size_t ExtendedEntrySize;

    void *Slots;
} HashTable;

#define entry_get_key(table, entry)      advance_bytes((entry), (table)->KeyOffset)
#define entry_get_value(table, entry)    advance_bytes((entry), (table)->ValueOffset)

#define entry_get_occupied(table, entry) ((bool *) advance_bytes((entry), (table)->PresenceFlagOffset))
#define entry_is_occupied(table, entry)  (*entry_get_occupied((table), (entry)))
#define entry_is_free(table, entry)      (false == *entry_get_occupied((table), (entry)))

static float load_factor(const HashTable *table) {
    return (float) table->UsedSlots / (float) table->TotalSlots;
}

static bool is_over_load_threshold(const HashTable *table) {
    return load_factor(table) >= LOAD_FACTOR_THRESHOLD;
}

static size_t hashtable_entry_index(const HashTable *table, const void *p_key) {
    return (size_t) table->Hash(p_key) % table->TotalSlots;
}

static void *hashtable_entry_at_index(const HashTable *table, size_t index) {
    index = index % table->TotalSlots;
    return advance_bytes(table->Slots, index * table->ExtendedEntrySize);
}

static void hashtable_set_entry(
        HashTable *table,
        void *entry,
        const void *p_key,
        const void *p_value
) {
    memcpy(entry_get_value(table, entry), p_value, table->ValueSize);
    if (*entry_get_occupied(table, entry)) {
        LOG_DEBUG("Key existed");
        return;
    }

    memcpy(entry_get_key(table, entry), p_key, table->KeySize);
    *entry_get_occupied(table, entry) = true;
    table->UsedSlots += 1;
    LOG_DEBUG("Inserted new key");
}

static void hashtable_put_ignore_load(HashTable *table, const void *p_key, const void *p_value) {
    assert(NULL != table);
    assert(NULL != p_key);
    assert(NULL != p_value);

    assert(!is_over_load_threshold(table));

    const size_t base_index = hashtable_entry_index(table, p_key);

    void *entry = hashtable_entry_at_index(table, base_index);
    size_t offset = 0;
    for (; offset < table->TotalSlots; offset++, entry = hashtable_entry_at_index(table, base_index + offset)) {
        if (entry_is_free(table, entry)) {
            LOG_DEBUG("Found empty slot");
            break;
        }

        if (0 == table->Compare(entry_get_key(table, entry), p_key)) {
            LOG_DEBUG("Found existing slot with the same key");
            break;
        }
    }

    hashtable_set_entry(table, entry, p_key, p_value);

    LOG_DEBUG(
            "Placed entry with hash %d to %d, slots_used=%d, load_factor=%.2lf",
            (int) base_index, (int) (base_index + offset), (int) table->UsedSlots, load_factor(table)
    );
}

static void *hashtable_replace_with_larger(HashTable *old_table) {
    const size_t new_slots_count = GROWTH_RATE * old_table->TotalSlots;
    const size_t total_size = old_table->ExtendedEntrySize * new_slots_count + sizeof(HashTable);

    HashTable *table = (HashTable *) calloc(1, total_size);
    if (NULL == table) {
        perror(nameof_identifier(hashtable_replace_with_larger));
        return NULL;
    }

    *table = *old_table;
    table->UsedSlots = 0;
    table->TotalSlots = new_slots_count;
    table->Slots = advance_bytes(table, sizeof(HashTable));

    LOG_DEBUG("Increased capacity from %d to %d", (int) old_table->TotalSlots, (int) table->TotalSlots);

    for (size_t i = 0; i < old_table->TotalSlots; i++) {
        void *old_entry = hashtable_entry_at_index(old_table, i);
        if (entry_is_free(old_table, old_entry)) {
            continue;
        }

        hashtable_put_ignore_load(
                table,
                entry_get_key(old_table, old_entry),
                entry_get_value(old_table, old_entry)
        );
    }
    free(old_table);


    return table;
}

static void *hashtable_entry_at_key(const HashTable *table, const void *p_key) {
    assert(NULL != table);
    assert(NULL != p_key);

    const size_t base_index = hashtable_entry_index(table, p_key);

    for (size_t offset = 0; offset < table->TotalSlots; offset++) {
        void *entry = hashtable_entry_at_index(table, base_index + offset);
        if (entry_is_free(table, entry)) {
            LOG_DEBUG("Found empty slot, key{hash=%d} does not exist", (int) base_index);
            return NULL;
        }

        if (0 == table->Compare(entry_get_key(table, entry), p_key)) {
            LOG_DEBUG("Located entry with key{hash=%d} at %d", (int) base_index, (int) (base_index + offset));
            return entry;
        }
    }

    LOG_DEBUG("key{hash=%d} does not exist", (int) base_index);
    return NULL;
}

void *hashtable_new(
        HashFunction hash,
        HashTableComparator compare,
        size_t slots_count,
        size_t key_size,
        size_t key_offset,
        size_t value_size,
        size_t value_offset,
        size_t entry_size,
        size_t entry_alignment
) {
    const size_t total_entry_size = entry_size + entry_alignment;
    const size_t total_size = slots_count * total_entry_size + sizeof(HashTable);
    HashTable *table = (HashTable *) calloc(1, total_size);
    if (NULL == table) {
        perror(nameof_identifier(hashtable_new));
        return NULL;
    }

    *table = (HashTable) {
            hash, compare,
            .TotalSlots = slots_count,
            .UsedSlots = 0,
            .KeySize = key_size, .KeyOffset = key_offset,
            .ValueSize = value_size, .ValueOffset = value_offset,
            .PresenceFlagOffset = entry_size,
            .ExtendedEntrySize = total_entry_size,
            .Slots = advance_bytes(table, sizeof(HashTable))
    };

    return table;
}

void hashtable_free(void *t, HashTableEntryDestructor free_entry) {
    HashTable *table = t;

    if (NULL != free_entry) {
        for (size_t i = 0; i < table->TotalSlots; i++) {
            void *entry = hashtable_entry_at_index(table, i);
            if (entry_is_free(table, entry)) {
                continue;
            }

            free_entry(entry);
        }
    }

    free(table);
}

void *hashtable_value_at(const void *t, const void *key) {
    if (NULL == t) {
        LOG_NULL(t);
        return NULL;
    }
    if (NULL == key) {
        LOG_NULL(key);
        return NULL;
    }

    const HashTable *table = t;
    void *entry = hashtable_entry_at_key(table, key);
    if (NULL == entry) {
        return NULL;
    }

    return entry_get_value(table, entry);
}

void *hashtable_put_entry(void *t, const void *entry) {
    if (NULL == t) {
        LOG_NULL(t);
        return NULL;
    }
    if (NULL == entry) {
        LOG_NULL(entry);
        return NULL;
    }

    HashTable *table = t;
    if (is_over_load_threshold(table)) {
        LOG_DEBUG("Exceeded load threshold, allocating more memory");

        if (NULL == (table = hashtable_replace_with_larger(table))) {
            LOG_ERROR("Could not increase table size");
            return NULL;
        }
    }

    hashtable_put_ignore_load(table, entry_get_key(table, entry), entry_get_value(table, entry));

    return table;
}

static size_t hashtable_next_used_slot_index(const HashTable *table, size_t base_index) {
    for (size_t index = base_index; index < table->TotalSlots; index++) {
        void *entry = hashtable_entry_at_index(table, index);
        if (entry_is_occupied(table, entry)) {
            return index;
        }
    }

    return table->TotalSlots;
}

size_t hashtable_size(const void *t) {
    const HashTable *table = t;
    return table->UsedSlots;
}

HashTableIterator hashtable_begin(const void *t) {
    const HashTable *table = t;
    return (HashTableIterator) {
            ._i =  hashtable_next_used_slot_index(table, 0)
    };
}

bool hashtable_end(const void *t, HashTableIterator it) {
    const HashTable *table = t;
    return it._i >= table->TotalSlots;
}

void hashtable_next(const void *t, HashTableIterator *it) {
    const HashTable *table = t;
    it->_i = hashtable_next_used_slot_index(table, it->_i + 1);
}

const void *hashtable_get_entry(const void *t, HashTableIterator it) {
    const HashTable *table = t;
    return hashtable_end(table, it) ? NULL : hashtable_entry_at_index(table, it._i);
}

void hashtable_copy_entry(const void *t, HashTableIterator it, void *dst) {
    const HashTable *table = t;
    const void *entry = hashtable_get_entry(table, it);
    if (NULL == entry) {
        return;
    }

    memcpy(dst, entry, table->KeyValueEntrySize);
}
