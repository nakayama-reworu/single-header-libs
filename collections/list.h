#pragma once

#include <stdlib.h>

#ifndef CallChecked
#error Please include call_checked.h before list.h
#endif

#define LIST_CONCAT_(A, B)   A ## B
#define LIST_CONCAT(A, B)    LIST_CONCAT_(A, B)

#define List_Of(TValue)                         \
struct LIST_CONCAT(List_, __LINE__) {           \
    TValue Value;                               \
    struct LIST_CONCAT(List_, __LINE__) *Next;  \
}

#define List_Free(HeadPtrPtr)           \
do {                                    \
    __auto_type _next = *(HeadPtrPtr);  \
    __auto_type _it = _next;            \
    while (NULL != _it) {               \
        _next = _it->Next;              \
        free(_it);                      \
        _it = _next;                    \
    }                                   \
    *(HeadPtrPtr) = NULL;               \
} while (0)

#define List_PushFront(HeadPtrPtr, Val)                 \
do {                                                    \
    __auto_type _v = (Val);                             \
    typeof(*(HeadPtrPtr)) _newHead =                    \
        CallChecked(calloc, (1, sizeof(*_newHead)));    \
    *_newHead = (typeof(*_newHead)) {                   \
        .Value = _v,                                    \
        .Next = *(HeadPtrPtr)                           \
    };                                                  \
    *(HeadPtrPtr) = _newHead;                           \
} while (0)

#define List_TryPopFront(HeadPtrPtr, ValuePtr)      \
({                                                  \
    bool _ok = false;                               \
    if (NULL != *(HeadPtrPtr)) {                    \
        *(ValuePtr) = (*(HeadPtrPtr))->Value;       \
        __auto_type _next = (*(HeadPtrPtr))->Next;  \
        free(*(HeadPtrPtr));                        \
        *(HeadPtrPtr) = _next;                      \
        _ok = true;                                 \
    }                                               \
    _ok;                                            \
})

#define List_PushBack(HeadPtrPtr, TailPtrPtr, Val)  \
do {                                                \
    if (NULL == *(HeadPtrPtr)) {                    \
        List_PushFront((HeadPtrPtr), (Val));        \
        *(TailPtrPtr) = *(HeadPtrPtr);              \
    } else {                                        \
        typeof(*(HeadPtrPtr)) _newTail = NULL;      \
        List_PushFront(&_newTail, (Val));           \
        (*(TailPtrPtr))->Next = _newTail;           \
        *(TailPtrPtr) = _newTail;                   \
    }                                               \
} while (0)

#define LIST_TYPEOF_MEMBER(Type, Member)    typeof(((Type) {}).Member)

#define List_ForEach(ValuePtr, HeadPtr)                                         \
__auto_type LIST_CONCAT(_it_, __LINE__) = (HeadPtr);                            \
for (                                                                           \
    LIST_TYPEOF_MEMBER(typeof(*LIST_CONCAT(_it_, __LINE__)), Value) *ValuePtr = \
        NULL != LIST_CONCAT(_it_, __LINE__)                                     \
            ? &LIST_CONCAT(_it_, __LINE__)->Value                               \
            : NULL;                                                             \
    NULL != LIST_CONCAT(_it_, __LINE__);                                        \
    LIST_CONCAT(_it_, __LINE__) = LIST_CONCAT(_it_, __LINE__)->Next,            \
    ValuePtr =                                                                  \
        NULL != LIST_CONCAT(_it_, __LINE__)                                     \
            ? &LIST_CONCAT(_it_, __LINE__)->Value                               \
            : NULL                                                              \
)

#define List_Tail(HeadPtr)                          \
({                                                  \
    __auto_type _tail = (HeadPtr);                  \
    while (NULL != _tail && NULL != _tail->Next) {  \
        _tail = _tail->Next;                        \
    }                                               \
    _tail;                                          \
})

#define List_Size(HeadPtr)                                              \
({                                                                      \
    size_t _size = 0;                                                   \
    for (__auto_type _it = (HeadPtr); NULL != _it; _it = _it->Next) {   \
        _size++;                                                        \
    }                                                                   \
    _size;                                                              \
})

#define List_Empty(HeadPtr) (NULL == (HeadPtr))
