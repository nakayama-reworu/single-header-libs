#ifndef PLAYGROUND_DYNARRAY_H
#define PLAYGROUND_DYNARRAY_H

#include "common/macros.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define ARRAY_MIN_CAPACITY 1

void *ArrayNew(size_t size, size_t element_size_bytes);

#define ARRAY_OF_TYPE(Type, Size)   (Type *) ArrayNew((Size), sizeof(Type))
#define ARRAY_EMPTY(Type)           (Type *) ArrayNew(/* size */ 0, sizeof(Type))

void ArrayFree(void *array);

#define ARRAY_FREE(Array) do { ArrayFree(Array); Array = NULL; } while (false)

size_t ArrayElementSize(const void *array);

size_t ArraySize(const void *array);

bool ArrayIsEmpty(const void *array);

size_t ArrayCapacity(const void *array);

void *ArrayReserveToFit(void *array, size_t size);

void ArrayFill(void *array, const void *value);

#define ARRAY_FILL(Array, Value)    ArrayFill((Array), ARRAY_LITERAL_FROM_VA_ARGS(Value))

#define ARRAY_AT(Array, I)  ((uint8_t *) (Array) + (I) * ArrayElementSize(Array))

void *ArrayAppend(void *array, const void *element);

#define ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral)    sizeof((ArrayLiteral)[0])
#define ARRAY_LITERAL_ELEMENTS_COUNT(ArrayLiteral)  (sizeof((ArrayLiteral)) / ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral))

#define ARRAY_APPEND(Array, Value) (Array = ArrayAppend((Array), ARRAY_LITERAL_FROM_VA_ARGS(Value)))

void *ArrayExtendWithValues(void *array, const void *data, size_t elements_count, size_t element_size);
#define ARRAY_EXTEND_WITH_ARRAY_LITERAL(Array, ArrayLiteral) \
    (Array = ArrayExtendWithValues( \
        Array, \
        ArrayLiteral, \
        ARRAY_LITERAL_ELEMENTS_COUNT(ArrayLiteral), \
        ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral) \
    ))

#define ARRAY_EXTEND_WITH(Array, ...) \
    ARRAY_EXTEND_WITH_ARRAY_LITERAL(Array, ARRAY_LITERAL_FROM_VA_ARGS(__VA_ARGS__))

void *ArrayExtend(void *array, const void *other);

void *ArrayOfValues(const void *data, size_t elements_count, size_t element_size);

#define ARRAY_FROM_ARRAY_LITERAL(ArrayLiteral) \
    ArrayOfValues( \
        ArrayLiteral, \
        ARRAY_LITERAL_ELEMENTS_COUNT(ArrayLiteral), \
        ARRAY_LITERAL_ELEMENT_SIZE(ArrayLiteral) \
    )

#define ARRAY_OF(...) \
    ARRAY_FROM_ARRAY_LITERAL(ARRAY_LITERAL_FROM_VA_ARGS(__VA_ARGS__))

bool ArrayPop(void *array, void *dst);

#define ARRAY_END(Array) (typeof(Array)) (ARRAY_AT(Array, ArraySize(Array)))

#define ARRAY_FOREACH(Array, Action) \
for (typeof(Array) _it = (Array); _it != ARRAY_END(Array); _it++) { \
    Action(_it); \
}

#endif //PLAYGROUND_DYNARRAY_H
