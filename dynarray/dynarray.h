#ifndef PLAYGROUND_DYNARRAY_H
#define PLAYGROUND_DYNARRAY_H

#include "common/macros.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef void (*ArrayElementDestructor)(void *);

#define ARRAY_MIN_CAPACITY 1

void *Array_New(size_t size, size_t element_size_bytes);

#define Array_OfType(type, size)   (type *) Array_New((size), sizeof(type))
#define Array_EmptyOfType(type)    (type *) Array_New(/* size */ 0, sizeof(type))

void Array_Free(void *array, ArrayElementDestructor free_element);

size_t Array_ElementSize(const void *array);

size_t Array_Size(const void *array);

bool Array_IsEmpty(const void *array);

size_t Array_Capacity(const void *array);

void *Array_ReserveToFit(void *array, size_t size);

#define Array_At(array, index)  SHIFT((array), (index) * Array_ElementSize(array))

void *Array_WithAppended(void *array, const void *element);

#define Array_Append(array, value) \
    (array = Array_WithAppended((array), array_literal_of_type(typeof(*array), value)))

void *Array_ExtendWithValues(void *array, const void *data, size_t elements_count, size_t element_size);

#define Array_ExtendWith(array, ...)        \
    (array = Array_ExtendWithValues(        \
        (array),                            \
        array_literal_sized(__VA_ARGS__),   \
        sizeof_first(__VA_ARGS__)           \
    ))

void *Array_Extend(void *array, const void *other);

void *Array_OfValues(const void *data, size_t elements_count, size_t element_size);

#define Array_Of(...)                       \
    Array_OfValues(                         \
        array_literal_sized(__VA_ARGS__),   \
        sizeof_first(__VA_ARGS__)           \
    )

bool Array_PopTo(void *array, void *dst);

#define Array_Pop(array)     \
({                              \
    typeof(*array) _item;       \
    Array_PopTo(array, &_item); \
    _item;                      \
})

#define Array_End(array) (typeof(array)) (Array_At(array, Array_Size(array)))

#define Array_ForEachElement(array, action)             \
do {                                                    \
    for (size_t _i = 0; _i < Array_Size(array); _i++) { \
        action(Array_At((array), _i));                  \
    }                                                   \
} while (false)

#define Array_ForEach(element_name, array)      \
for (                                           \
    typeof(*array) *p_##element_name = array,   \
        element_name = (Array_IsEmpty(array)    \
            ? (typeof(*array)) {}               \
            : *p_##element_name                 \
        );                                      \
    p_##element_name != Array_End(array);       \
    p_##element_name++,                         \
        (p_##element_name != Array_End(array)   \
            ? (element_name = *p_##element_name)\
            : element_name                      \
        )                                       \
)

#endif //PLAYGROUND_DYNARRAY_H
