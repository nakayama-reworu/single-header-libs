#ifndef PLAYGROUND_DYNARRAY_H
#define PLAYGROUND_DYNARRAY_H

#include "common/macros.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define ARRAY_MIN_CAPACITY 1

void *Array_New(size_t size, size_t element_size_bytes);

#define Array_OfType(Type, Size)   (Type *) Array_New((Size), sizeof(Type))
#define Array_EmptyOfType(Type)    (Type *) Array_New(/* size */ 0, sizeof(Type))

void Array_Free(void *array);

#define Array_FreeAndSetToNull(Array) do { Array_Free(Array); Array = NULL; } while (false)

size_t Array_ElementSize(const void *array);

size_t Array_Size(const void *array);

bool Array_IsEmpty(const void *array);

size_t Array_Capacity(const void *array);

void *Array_ReserveToFit(void *array, size_t size);

void Array_FillFrom(void *array, const void *value);

#define Array_Fill(Array, Value)    Array_FillFrom((Array), ARRAY_LITERAL_FROM_VA_ARGS(Value))

#define Array_At(Array, I)  ((uint8_t *) (Array) + (I) * Array_ElementSize(Array))

void *Array_AppendFrom(void *array, const void *element);

#define ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral)    sizeof((ArrayLiteral)[0])
#define ARRAY_LITERAL_ELEMENTS_COUNT(ArrayLiteral)  (sizeof((ArrayLiteral)) / ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral))

#define Array_Append(Array, Value) (Array = Array_AppendFrom((Array), ARRAY_LITERAL_FROM_VA_ARGS(Value)))

void *Array_ExtendWithValues(void *array, const void *data, size_t elements_count, size_t element_size);

#define ARRAY_EXTEND_WITH_ARRAY_LITERAL(Array, ArrayLiteral) \
    (Array = Array_ExtendWithValues( \
        Array, \
        ArrayLiteral, \
        ARRAY_LITERAL_ELEMENTS_COUNT(ArrayLiteral), \
        ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral) \
    ))

#define Array_ExtendWith(Array, ...) \
    ARRAY_EXTEND_WITH_ARRAY_LITERAL(Array, ARRAY_LITERAL_FROM_VA_ARGS(__VA_ARGS__))

void *Array_Extend(void *array, const void *other);

void *Array_OfValues(const void *data, size_t elements_count, size_t element_size);

#define ARRAY_FROM_ARRAY_LITERAL(ArrayLiteral) \
    Array_OfValues( \
        ArrayLiteral, \
        ARRAY_LITERAL_ELEMENTS_COUNT(ArrayLiteral), \
        ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral) \
    )

#define Array_Of(...) \
    ARRAY_FROM_ARRAY_LITERAL(ARRAY_LITERAL_FROM_VA_ARGS(__VA_ARGS__))

bool Array_Pop(void *array, void *dst);

#define Array_End(Array) (typeof(Array)) (Array_At(Array, Array_Size(Array)))

#define Array_ForEachElement(Array, Action) \
do { \
    for (size_t _i = 0; _i < Array_Size(Array); _i++) { \
        Action(Array_At((Array), _i)); \
    } \
} while (false)

#endif //PLAYGROUND_DYNARRAY_H
