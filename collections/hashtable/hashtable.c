#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define Pair(key_type, value_type)      pair_##key_type##_##value_type

#define pair_declare(key_type, value_type)  \
typedef struct {                            \
    key_type key;                           \
    value_type value;                       \
} Pair(key_type, value_type)

#define typeof_member(type, member) typeof(((type){0}).member)

typedef size_t (*Hash)(const void *);

typedef int (*Compare)(const void *, const void *);

#define print_error(failed_call)                \
fprintf(                                        \
    stderr,                                     \
    "%s:%d (%s) `%s` failed: %s (errno=%d)\n",  \
    __FILE_NAME__, __LINE__, __FUNCTION__,      \
    ({ (void) failed_call; #failed_call; }),    \
    strerror(errno), errno                      \
)

#ifdef assert
#undef assert
#endif
#define assert_always(expr)                         \
do {                                                \
    if (expr) { break; }                            \
    fprintf(                                        \
        stderr,                                     \
        "%s:%d (%s) Assertion failed: `" #expr "`", \
        __FILE_NAME__, __LINE__, __FUNCTION__       \
    );                                              \
    exit(EXIT_FAILURE);                             \
} while (0)

#include <assert.h>

#define on_malloc_error() exit(EXIT_FAILURE)

#define HASHTABLE_DEFAULT_CAPACITY  (32)
#define LOAD_FACTOR_THRESHOLD       (0.5f)

typedef struct Header {
    Hash hash;
    Compare compare;

    size_t key_size;

    size_t value_size;
    size_t value_offset;

    size_t entry_size;

    size_t size;
    size_t capacity;

    bool *used;
    uint8_t data[];
} Header;

static Header *header_get(void *hashtable) { return (Header *) hashtable - 1; }

static const Header *header_get_const(const void *hashtable) { return (const Header *) hashtable - 1; }

size_t hashtable_size(const void *hashtable) { return header_get_const(hashtable)->size; }
size_t hashtable_capacity(const void *hashtable) { return header_get_const(hashtable)->capacity; }

void *hashtable_new(
        Hash hash,
        Compare compare,
        size_t key_size,
        size_t value_size,
        size_t value_offset,
        size_t entry_size,
        size_t capacity
) {
    if (0 == capacity) {
        capacity = HASHTABLE_DEFAULT_CAPACITY;
    }

    const size_t sizeof_entries = entry_size * capacity;
    const size_t sizeof_used_flags = sizeof(bool) * capacity;
    const size_t total_size = sizeof(Header) + sizeof_entries + sizeof_used_flags;

    //  Header | Entries | Flags
    Header *header = calloc(1, total_size);
    if (NULL == header) {
        print_error("calloc");
        on_malloc_error();
    }

    header->hash = hash;
    header->compare = compare;
    header->key_size = key_size;
    header->value_size = value_size;
    header->value_offset = value_offset;
    header->entry_size = entry_size;
    header->size = 0;
    header->capacity = capacity;
    header->used = (bool *) (header->data + sizeof_entries);

    return header->data;
}

#define hashtable_of_type(entry_type, hash, compare)    \
(entry_type *) hashtable_new(                           \
    hash, compare,                                      \
    sizeof(((entry_type){0}).key),                      \
    sizeof(((entry_type){0}).value),                    \
    offsetof(entry_type, value),                        \
    sizeof(entry_type),                                 \
    HASHTABLE_DEFAULT_CAPACITY                          \
)

void hashtable_free(void *hashtable) {
    free(header_get(hashtable));
}

static size_t key_hash(const Header *header, const void *key) {
    return header->hash(key) / header->capacity;
}

static bool key_equals(const Header *header, const void *key1, const void *key2) {
    return 0 == header->compare(key1, key2);
}

#define entry_at(header, index)     ((header)->data + (index) * (header)->entry_size)
#define entry_key(header, entry)    ((entry))
#define entry_value(header, entry)  ((entry) + (header)->value_offset)

static bool entry_find_index(const Header *header, const void *key, size_t *index) {
    assert_always(header->size < header->capacity);

    const size_t hash = key_hash(header, key);

    for (size_t i = 0; i < header->capacity; i += 1) {
        const size_t entry_index = (hash + i) % header->capacity;

        if (false == header->used[entry_index]) {
            *index = entry_index;
            return false;
        }

        const uint8_t *entry = entry_at(header, entry_index);
        if (key_equals(header, key, entry_key(header, entry))) {
            *index = entry_index;
            return true;
        }
    }

    assert_always(false && "UNREACHABLE");
}

void *hashtable_value_at(void *hashtable, const void *key) {
    Header *header = header_get(hashtable);
    size_t index;
    if (false == entry_find_index(header, key, &index)) {
        return NULL;
    }

    uint8_t *entry = entry_at(header, index);
    return entry_value(header, entry);
}

#define hashtable_at(ht, k)                                 \
(typeof_member(typeof(*(ht)), value) *) hashtable_value_at( \
    (ht), (typeof_member(typeof(*(ht)), key)[]){k}          \
)

bool hashtable_copy_value_to(const void *hashtable, const void *key, void *dst) {
    const Header *header = header_get_const(hashtable);
    size_t index;
    if (false == entry_find_index(header, key, &index)) {
        return false;
    }

    const uint8_t *entry = entry_at(header, index);
    memcpy(dst, entry_value(header, entry), header->value_size);
    return true;
}

#define hashtable_get(ht, k)                        \
({                                                  \
    typeof_member(typeof(*(ht)), value) _v = {};    \
    assert(hashtable_copy_value_to(                 \
        (ht),                                       \
        (typeof_member(typeof(*(ht)), key)[]){k},   \
        &_v                                         \
    ));                                             \
    _v;                                             \
})

static void *hashtable_put_ignore_load(Header *header, const void *key, const void *value) {
    size_t entry_index;
    const bool exists = entry_find_index(header, key, &entry_index);

    uint8_t *entry = entry_at(header, entry_index);
    if (false == exists) {
        memcpy(entry_key(header, entry), key, header->key_size);
    }
    memcpy(entry_value(header, entry), value, header->value_size);

    header->used[entry_index] = true;
    header->size += 1;

    return entry_value(header, entry);
}

static float load_factor(const Header *header) {
    return (float) header->size / (float) header->capacity;
}

static Header *realloc_hashtable(Header *old) {
    const size_t new_capacity = old->capacity * 3 / 2 + 1;
    void *new_table = hashtable_new(
            old->hash,
            old->compare,
            old->key_size,
            old->value_size,
            old->value_offset,
            old->entry_size,
            new_capacity
    );
    if (NULL == new_table) {
        print_error("calloc");
        on_malloc_error();
    }
    Header *new = header_get(new_table);

    for (size_t i = 0; i < old->capacity; i += 1) {
        if (false == old->used[i]) {
            continue;
        }

        uint8_t *entry = entry_at(old, i);
        hashtable_put_ignore_load(new, entry_key(old, entry), entry_value(old, entry));
    }

    free(old);
    return new;
}

void *hashtable_with_entry(void *hashtable, const void *key, const void *value, void **created_value) {
    Header *header = header_get(hashtable);
    if (load_factor(header) >= LOAD_FACTOR_THRESHOLD) {
        if (NULL == (header = realloc_hashtable(header))) {
            return NULL;
        }
    }

    if (NULL != created_value) {
        *created_value = hashtable_put_ignore_load(header, key, value);
    }

    return header->data;
}

#define hashtable_put(ht, k, v)                     \
({                                                  \
    void *p_v;                                      \
    (ht) = hashtable_with_entry(                    \
        (ht),                                       \
        (typeof_member(typeof(*(ht)), key)[]){k},   \
        (typeof_member(typeof(*(ht)), value)[]){v}, \
        &p_v                                        \
    );                                              \
    (typeof_member(typeof(*(ht)), value) *) p_v;    \
})

size_t int_hash(const void *p) {
    // https://stackoverflow.com/a/12996028/17654649
    int x = *(int *) p;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

int int_compare(const void *p1, const void *p2) {
    int *i1 = (int *) p1, *i2 = (int *) p2;
    return *i1 - *i2;
}

pair_declare(int, int);

int main(void) {
    Pair(int, int) *table = hashtable_of_type(Pair(int, int), int_hash, int_compare);

    const size_t key_max = 20;

    for (size_t i = 1; i <= key_max; i += 1) {
        int *value = hashtable_put(table, i, i - 1);
        *value += 1;
    }
    for (size_t i = 1; i <= key_max; i += 1) {
        int *value = hashtable_at(table, i);
        *value *= *value;
    }
    for (size_t i = 1; i <= key_max; i += 1) {
        printf("[%zu] = %d\n", i, hashtable_get(table, i));
    }

    printf("size     = %zu\n", hashtable_size(table));
    printf("capacity = %zu\n", hashtable_capacity(table));

    hashtable_free(table);

    return EXIT_SUCCESS;
}
