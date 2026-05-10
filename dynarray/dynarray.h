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

void Array_FillFrom(void *array, const void *value);

#define Array_Fill(Array, Value)    Array_FillFrom((Array), ARRAY_LITERAL_FROM_VA_ARGS(Value))

#define Array_At(array, index)  SHIFT((array), (index) * Array_ElementSize(array))

void *Array_WithAppended(void *array, const void *element);

#define ARRAY_LITERAL_ELEMENT_SIZE(array_literal)    sizeof((array_literal)[0])
#define ARRAY_LITERAL_ELEMENTS_COUNT(array_literal)  (sizeof((array_literal)) / ARRAY_LITERAL_ELEMENT_SIZE(array_literal))

#define Array_Append(array, value) \
    (array = Array_WithAppended((array), ARRAY_LITERAL_FROM_VA_ARGS_TYPE(typeof(*array), value)))

void *Array_ExtendWithValues(void *array, const void *data, size_t elements_count, size_t element_size);

#define ARRAY_EXTEND_WITH_ARRAY_LITERAL(array, array_literal) \
    (array = Array_ExtendWithValues( \
        array, \
        array_literal, \
        ARRAY_LITERAL_ELEMENTS_COUNT(array_literal), \
        ARRAY_LITERAL_ELEMENT_SIZE(array_literal) \
    ))

#define Array_ExtendWith(array, ...) \
    ARRAY_EXTEND_WITH_ARRAY_LITERAL(array, ARRAY_LITERAL_FROM_VA_ARGS(__VA_ARGS__))

void *Array_Extend(void *array, const void *other);

void *Array_OfValues(const void *data, size_t elements_count, size_t element_size);

#define ARRAY_FROM_ARRAY_LITERAL(array_literal) \
    Array_OfValues( \
        array_literal, \
        ARRAY_LITERAL_ELEMENTS_COUNT(array_literal), \
        ARRAY_LITERAL_ELEMENT_SIZE(array_literal) \
    )

#define Array_Of(...) \
    ARRAY_FROM_ARRAY_LITERAL(ARRAY_LITERAL_FROM_VA_ARGS(__VA_ARGS__))

bool Array_Pop(void *array, void *dst);

#define Array_End(array) (typeof(array)) (Array_At(array, Array_Size(array)))

#define Array_ForEachElement(array, action)             \
do {                                                    \
    for (size_t _i = 0; _i < Array_Size(array); _i++) { \
        action(Array_At((array), _i));                  \
    }                                                   \
} while (false)

#define Array_ForEach(element_name, array)  \
typeof(*array) element_name;                \
if (false == Array_IsEmpty(array)) {        \
    element_name = array[0];                \
}                                           \
for (                                       \
    size_t _i = 0;                          \
    _i < Array_Size(array);                 \
    _i++,                                   \
    element_name = _i < Array_Size(array)   \
        ? array[_i]                         \
        : element_name                      \
)

#endif //PLAYGROUND_DYNARRAY_H
