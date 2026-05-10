#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdbool.h>

void *vector_new(size_t element_size, size_t elements_count);

#define vector_new_of_type(type, size)  ((type *) (vector_new(sizeof(type), size)))
#define vector_empty_of_type(type)      vector_new_of_type(type, 0)

void vector_free_(void *vector);

#define vector_free(vector) \
do {                        \
    vector_free_(vector);   \
    (vector) = NULL;        \
} while (0)

const void *vector_end_(const void *vector);

#define vector_end(vector) ((typeof(vector)) vector_end_(vector))

size_t vector_size(const void *);

bool vector_empty(const void *);

void *vector_with_appended_from(void *, const void *value);

#define vector_append(vector, value)        \
do {                                        \
    typeof(vector) _tmp =                   \
        vector_with_appended_from(          \
            (vector),                       \
            (typeof(*vector)[]) {(value)}   \
        );                                  \
    assert(NULL != _tmp);                   \
    (vector) = _tmp;                        \
} while (0)

void *vector_with_range_appended_from(void *, const void *src, size_t count);

#define vector_append_range(vector, src, count) \
do {                                            \
    typeof(vector) _src = (src);                \
    typeof(vector) _tmp =                       \
        vector_with_range_appended_from(        \
            (vector),                           \
            (_src), (count)                     \
        );                                      \
    assert(NULL != _tmp);                       \
    (vector) = _tmp;                            \
} while (0)

#define vector_append_values(vector, ...)                   \
do {                                                        \
    typeof(*(vector)) _args[] = {__VA_ARGS__};              \
    const size_t _count = sizeof(_args) / sizeof(_args[0]); \
    vector_append_range((vector), _args, _count);           \
} while (0)\


bool vector_pop_to(void *, void *dst);

#define vector_pop(vector)                     \
({                                             \
    typeof(*(vector)) _val = {0};              \
    assert(vector_pop_to((vector), &_val));    \
    _val;                                      \
})

#define vector_foreach(p_element, vector) \
for (typeof(vector) p_element = (vector); p_element < vector_end(vector); p_element++)

#endif // VECTOR_H
