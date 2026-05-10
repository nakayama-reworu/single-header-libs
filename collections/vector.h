#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

#define VECTOR__Concat_(A, B)   A ## B
#define VECTOR__Concat(A, B)    VECTOR__Concat_(A, B)

#define Vector_Of(TValue) \
struct {                  \
    size_t Size;          \
    size_t Capacity;      \
    TValue *Items;        \
}

#define Vector_Free(VecPtr)                 \
do {                                        \
    free((void *) (VecPtr)->Items);         \
    *(VecPtr) = (typeof(*(VecPtr))) { 0 };  \
} while (0)

#define Vector_PushBack(VecPtr, Val)                                        \
do {                                                                        \
    __auto_type _vec_push_back = (VecPtr);                                  \
    if (_vec_push_back->Size >= _vec_push_back->Capacity) {                 \
        _vec_push_back->Capacity =                                          \
            3 * _vec_push_back->Capacity / 2 + 1;                           \
        __auto_type _items = VECTOR__CallChecked(realloc, (                 \
            _vec_push_back->Items,                                          \
            _vec_push_back->Capacity * sizeof(_vec_push_back->Items[0])     \
        ));                                                                 \
        _vec_push_back->Items = _items;                                     \
    }                                                                       \
    _vec_push_back->Items[_vec_push_back->Size++] = (Val);                  \
} while (0)

#define Vector_TryPopBack(VecPtr, ValuePtr)                         \
({                                                                  \
    __auto_type _vec_try_pop_back = (VecPtr);                       \
    bool _ok = false;                                               \
    if (_vec_try_pop_back->Size > 0) {                              \
        *(ValuePtr) =                                               \
            _vec_try_pop_back->Items[--_vec_try_pop_back->Size];    \
        _ok = true;                                                 \
    }                                                               \
    _ok;                                                            \
})

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

#define Vector_ForEach(ValuePtr, Vec)                                                       \
size_t VECTOR__Concat(_it_, __LINE__) = 0;                                                  \
__auto_type VECTOR__Concat(_vec_for_, __LINE__) = (Vec);                                    \
for (                                                                                       \
    typeof(*(VECTOR__Concat(_vec_for_, __LINE__).Items)) *ValuePtr =                        \
        VECTOR__Concat(_it_, __LINE__) >= VECTOR__Concat(_vec_for_, __LINE__).Size          \
            ? NULL                                                                          \
            : &VECTOR__Concat(_vec_for_, __LINE__).Items[VECTOR__Concat(_it_, __LINE__)];   \
    VECTOR__Concat(_it_, __LINE__) < VECTOR__Concat(_vec_for_, __LINE__).Size;              \
    VECTOR__Concat(_it_, __LINE__)++,                                                       \
    ValuePtr =                                                                              \
        VECTOR__Concat(_it_, __LINE__) >= VECTOR__Concat(_vec_for_, __LINE__).Size          \
            ? NULL                                                                          \
            : &VECTOR__Concat(_vec_for_, __LINE__).Items[VECTOR__Concat(_it_, __LINE__)]    \
)

#define Vector_Empty(Vec)       (0 == (Vec).Size)

#define Vector_Clear(VecPtr)    do { (VecPtr)->Size = 0; } while (0)

#define Vector_Any(Vec, Value, PredicateExpr)   \
({                                              \
    bool _matches = false;                      \
    Vector_ForEach(p, (Vec)) {                  \
        __auto_type Value = *p;                 \
        if ((PredicateExpr)) {                  \
            _matches = true;                    \
            break;                              \
        }                                       \
    }                                           \
    _matches;                                   \
})

#define VectorSlice_Of(TValue)    \
struct {                    \
    size_t Size;            \
    TValue *Items;          \
}

#define Vector_Slice(TSlice, Vec, Start, End)           \
({                                                      \
    __auto_type _vec_slice = (Vec);                     \
    long long _sz_slice = _vec_slice.Size;              \
    long long _start = (Start);                         \
    if (_start < 0) _start = 0;                         \
    if (_start > _sz_slice) _start = _sz_slice;         \
    long long _end = (End);                             \
    if (_end < 0) _end = 0;                             \
    if (_end > _sz_slice) _end = _sz_slice;             \
    __auto_type _items = Vector_At(_vec_slice, _start); \
    bool _ok = 0 <= _start                              \
        && _start < _sz_slice                           \
        && _end >= _start                               \
        && _end <= _sz_slice;                           \
    (TSlice) {                                          \
        .Size=(_ok ? (_end-_start) : 0),                \
        .Items=(_ok ? _items : NULL),                   \
    };                                                  \
})

#define Vector_SliceFrom(TSlice, Vec, Start)                                \
({                                                                          \
    __auto_type _vec_slice_from = (Vec);                                    \
    Vector_Slice(TSlice, _vec_slice_from, (Start), _vec_slice_from.Size);   \
})

#define Vector_SliceTo(TSlice, Vec, End)            \
({                                                  \
    __auto_type _vec_slice_to = (Vec);              \
    Vector_Slice(TSlice, _vec_slice_to, 0, (End));  \
})

#endif // VECTOR_H
