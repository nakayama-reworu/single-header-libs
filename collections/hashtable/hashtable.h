#ifndef PLAYGROUND_HASHTABLE_H
#define PLAYGROUND_HASHTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdalign.h>
#include <assert.h>

#define Pair(key_type, value_type)      pair_##key_type##_##value_type

#define pair_declare(key_type, value_type)  \
typedef struct {                            \
    const key_type key;                     \
    value_type value;                       \
} Pair(key_type, value_type)

typedef size_t (*Hash)(const void *);

typedef int (*Compare)(const void *, const void *);

size_t hashtable_size(const void *hashtable);

bool hashtable_empty(const void *hashtable);

size_t hashtable_capacity(const void *hashtable);

#define HASHTABLE_DEFAULT_CAPACITY  (32)

void *hashtable_new(
        Hash hash,
        Compare compare,
        size_t key_size,
        size_t key_offset,
        size_t value_size,
        size_t value_offset,
        size_t entry_size,
        size_t capacity
);

#define hashtable_of_type(entry_type, hash, compare)    \
(entry_type *) hashtable_new(                           \
    hash, compare,                                      \
    sizeof((entry_type){}.key),                         \
    offsetof(entry_type, key),                          \
    sizeof((entry_type){}.value),                       \
    offsetof(entry_type, value),                        \
    sizeof(entry_type),                                 \
    HASHTABLE_DEFAULT_CAPACITY                          \
)

void hashtable_free(void *hashtable);

void *hashtable_value_at(void *hashtable, const void *key);

#define hashtable_at(ht, k)                             \
(typeof((typeof(*(ht))){}.value) *) hashtable_value_at( \
    (ht), (typeof((typeof(*(ht))){}.key)[]){k}        \
)

bool hashtable_copy_value_to(const void *hashtable, const void *key, void *dst);

#define hashtable_get(ht, k)                        \
({                                                  \
    typeof((typeof(*(ht))){}.value) _v = {};        \
    assert(hashtable_copy_value_to(                 \
        (ht),                                       \
        (typeof((typeof(*(ht))){}.key)[]){k},       \
        &_v                                         \
    ));                                             \
    _v;                                             \
})

void *hashtable_with_entry(void *hashtable, const void *key, const void *value, void **created_value);

#define hashtable_put(ht, k, v)                     \
({                                                  \
    void *p_v;                                      \
    (ht) = hashtable_with_entry(                    \
        (ht),                                       \
        (typeof((typeof(*(ht))){0}.key)[]){k},      \
        (typeof((typeof(*(ht))){0}.value)[]){v},    \
        &p_v                                        \
    );                                              \
    (typeof((typeof(*(ht))){0}.value) *) p_v;       \
})

size_t hashtable_begin(const void *hashtable);

size_t hashtable_end(const void *hashtable);

size_t hashtable_next(const void *hashtable, size_t i);

#define ht_concat_1_(x, y)   x##y
#define ht_concat_(x, y)     ht_concat_1_(x, y)

#define hashtable_foreach(p_element, ht)                                        \
size_t ht_concat_(_i_, __LINE__) = hashtable_begin(ht);                         \
for (                                                                           \
    typeof(ht) p_element = ht + ht_concat_(_i_, __LINE__);                      \
    ht_concat_(_i_, __LINE__) < hashtable_end(ht);                              \
    ht_concat_(_i_, __LINE__) = hashtable_next(ht, ht_concat_(_i_, __LINE__)),  \
            p_element = ht + ht_concat_(_i_, __LINE__)                          \
)

#endif // PLAYGROUND_HASHTABLE_H
