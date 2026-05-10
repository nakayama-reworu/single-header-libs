#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "call_checked.h"

/*
 * Vector is a struct { size_t Size; size_t Capacity; T *Items; }
 */

#ifndef CallChecked
#error 'CallChecked' is required by this implementation
#endif

#ifndef CONCAT
#define CONCAT_(A, B)   A ## B
#define CONCAT(A, B)    CONCAT_(A, B)
#endif

#define Vector_Of(TValue) struct { size_t Size; size_t Capacity; TValue *Items; }

#define Vector_Free(VecPtr) do { free((void *) (VecPtr)->Items); *(VecPtr) = (typeof(*(VecPtr))) { 0 }; } while (0)

#define Vector_PushBack(VecPtr, Val)                            \
do {                                                            \
    if ((VecPtr)->Size >= (VecPtr)->Capacity) {                 \
        (VecPtr)->Capacity =                                    \
            3 * (VecPtr)->Capacity / 2 + 1;                     \
        __auto_type _items = CallChecked(realloc, (             \
            (VecPtr)->Items,                                    \
            (VecPtr)->Capacity * sizeof((VecPtr)->Items[0])     \
        ));                                                     \
        (VecPtr)->Items = _items;                               \
    }                                                           \
    (VecPtr)->Items[(VecPtr)->Size++] = (Val);                  \
} while (0)

#define Vector_TryPopBack(VecPtr, ValuePtr) \
({                                                  \
    bool _ok = false;                               \
    if ((VecPtr)->Size > 0) {                       \
        *(ValuePtr) =                               \
            (VecPtr)->Items[--(VecPtr)->Size];      \
        _ok = true;                                 \
    }                                               \
    _ok;                                            \
})

#define Vector_ForEach(ValuePtr, Vec)                   \
size_t CONCAT(_it_, __LINE__) = 0;                      \
for (                                                   \
    typeof(*((Vec).Items)) *ValuePtr =                  \
        CONCAT(_it_, __LINE__) >= (Vec).Size            \
            ? NULL                                      \
            : &(Vec).Items[CONCAT(_it_, __LINE__)];     \
    CONCAT(_it_, __LINE__) < (Vec).Size;                \
    CONCAT(_it_, __LINE__)++,                           \
    ValuePtr =                                          \
        CONCAT(_it_, __LINE__) >= (Vec).Size            \
            ? NULL                                      \
            : &(Vec).Items[CONCAT(_it_, __LINE__)]      \
)

int main(void) {
    Vector_Of(int) a = {0};

    Vector_PushBack(&a, 1);
    Vector_PushBack(&a, 2);
    Vector_PushBack(&a, 3);

    Vector_ForEach(pValue, a) {
        printf("%d\n", *pValue);
    }

    int value;
    while (Vector_TryPopBack(&a, &value)) {
        printf("%d\n", value);
    }

    printf("%zu %zu %p\n", a.Size, a.Capacity, a.Items);
    Vector_Free(&a);
    printf("%zu %zu %p\n", a.Size, a.Capacity, a.Items);

    return EXIT_SUCCESS;
}