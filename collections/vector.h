#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VECTOR__ArrayLength(Array)      (sizeof(Array) / sizeof(*(Array)))
#define VECTOR__ToArrayLiteral(_0, ...) ((typeof(_0)[]) {_0, ##__VA_ARGS__})

#define VECTOR__CallChecked(Callee, ArgsList)   \
({                                          \
    errno = 0;                              \
    __auto_type _r = Callee ArgsList;       \
    if (errno) {                            \
        fprintf(                            \
            stderr, "[%s:%d] %s%s: %s\n",   \
            __FILE_NAME__, __LINE__,        \
            #Callee, #ArgsList,             \
            strerror(errno)                 \
        );                                  \
        exit(EXIT_FAILURE);                 \
    }                                       \
    _r;                                     \
})

#define Vector(TValue)  \
struct {                \
    size_t Size;        \
    size_t Capacity;    \
    TValue *Items;      \
}

#define Vector_Empty(VectorType) ((VectorType) {0})

#define Vector_FromPtr(VectorType, Ptr, Count)              \
({                                                          \
    VectorType _vec_fromArray = Vector_Empty(VectorType);   \
    typeof(*_vec_fromArray.Items) *_arr = (Ptr);            \
    size_t const _count = (Count);                          \
    for (size_t _i = 0; _i < _count; _i++) {                \
        Vector_PushBack(&_vec_fromArray, _arr[_i]);         \
    }                                                       \
    _vec_fromArray;                                         \
})

#define Vector_FromArray(VectorType, Array) Vector_FromPtr(VectorType, (Array), VECTOR__ArrayLength(Array))

#define Vector_From(VectorType, Src)                                \
({                                                                  \
    __auto_type _src_from = (Src);                                  \
    Vector_FromPtr(VectorType, _src_from.Items, _src_from.Size);    \
})

#define Vector_Of(VectorType, ...) Vector_FromArray(VectorType, VECTOR__ToArrayLiteral(__VA_ARGS__))

#define Vector_Free(VecPtr)                 \
do {                                        \
    free((void *) (VecPtr)->Items);         \
    *(VecPtr) = (typeof(*(VecPtr))) { 0 };  \
} while (0)

#define Vector_Reserve(VecPtr, NewCapacity)                             \
do {                                                                    \
    size_t _newCapacity = (NewCapacity);                                \
    __auto_type _vecPtr_reserve = (VecPtr);                             \
    if (_newCapacity <= _vecPtr_reserve->Capacity) {                    \
        break;                                                          \
    }                                                                   \
    _vecPtr_reserve->Capacity = _newCapacity;                           \
    __auto_type _items = VECTOR__CallChecked(realloc, (                 \
        _vecPtr_reserve->Items,                                         \
        _vecPtr_reserve->Capacity * sizeof(_vecPtr_reserve->Items[0])   \
    ));                                                                 \
    _vecPtr_reserve->Items = _items;                                    \
} while (0)

#define Vector_PushBack(VecPtr, Val)                                        \
do {                                                                        \
    __auto_type _vecPtr_pushBack = (VecPtr);                                \
    if (_vecPtr_pushBack->Size >= _vecPtr_pushBack->Capacity) {             \
        _vecPtr_pushBack->Capacity =                                        \
            3 * _vecPtr_pushBack->Capacity / 2 + 1;                         \
        Vector_Reserve(                                                     \
            _vecPtr_pushBack,                                               \
            3 * _vecPtr_pushBack->Capacity / 2 + 1                          \
        );                                                                  \
    }                                                                       \
    _vecPtr_pushBack->Items[_vecPtr_pushBack->Size++] = (Val);              \
} while (0)

#define Vector_TryPopBack(VecPtr, ValuePtr)                                 \
({                                                                          \
    __auto_type _vecPtr_tryPopBack = (VecPtr);                              \
    typeof(_vecPtr_tryPopBack->Items) _valuePtr_tryPopBack = (ValuePtr);    \
    bool _ok = false;                                                       \
    if (_vecPtr_tryPopBack->Size > 0) {                                     \
        --_vecPtr_tryPopBack->Size;                                         \
        if (NULL != _valuePtr_tryPopBack) {                                 \
            *_valuePtr_tryPopBack =                                         \
                _vecPtr_tryPopBack->Items[_vecPtr_tryPopBack->Size];        \
        }                                                                   \
        _ok = true;                                                         \
    }                                                                       \
    _ok;                                                                    \
})

#define Vector_Reverse(VecPtr)                                                      \
do {                                                                                \
    __auto_type _vecPtr_reverse = (VecPtr);                                         \
    __auto_type _low_reverse = _vecPtr_reverse->Items;                              \
    __auto_type _high_reverse = _vecPtr_reverse->Items + _vecPtr_reverse->Size - 1; \
    while (_low_reverse < _high_reverse) {                                          \
        __auto_type _tmp = *_low_reverse;                                           \
        *_low_reverse = *_high_reverse;                                             \
        *_high_reverse = _tmp;                                                      \
        _low_reverse++;                                                             \
        _high_reverse--;                                                            \
    }                                                                               \
} while (0)

#define Vector_IsEmpty(Vec) (0 == (Vec).Size)

#define Vector_At(Vec, Index)                   \
({                                              \
    __auto_type _vec_at = (Vec);                \
    long long _idx = (Index);                   \
    long long _sz = _vec_at.Size;               \
    typeof(_vec_at.Items[0]) *_value = NULL;    \
    if (0 <= _idx && _idx < _sz) {              \
        _value = _vec_at.Items + _idx;          \
    }                                           \
    if (-_sz <= _idx && _idx < 0) {             \
        _value = _vec_at.Items + _sz + _idx;    \
    }                                           \
    _value;                                     \
})

#define Vector_Clear(VecPtr) do { (VecPtr)->Size = 0; } while (0)

#endif // VECTOR_H
