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

#define List_PushFront(HeadPtrPtr, Val)                     \
do {                                                        \
    __auto_type _head_ptr_ptr_push_front = (HeadPtrPtr);    \
    __auto_type _v = (Val);                                 \
    typeof(*_head_ptr_ptr_push_front) _newHead =            \
        CallChecked(calloc, (1, sizeof(*_newHead)));        \
    *_newHead = (typeof(*_newHead)) {                       \
        .Value = _v,                                        \
        .Next = *_head_ptr_ptr_push_front                   \
    };                                                      \
    *_head_ptr_ptr_push_front = _newHead;                   \
} while (0)

#define List_TryPopFront(HeadPtrPtr, ValuePtr)                      \
({                                                                  \
    __auto_type _head_ptr_ptr_try_pop_front = (HeadPtrPtr);         \
    bool _ok = false;                                               \
    if (NULL != *_head_ptr_ptr_try_pop_front) {                     \
        *(ValuePtr) = (*_head_ptr_ptr_try_pop_front)->Value;        \
        __auto_type _next = (*_head_ptr_ptr_try_pop_front)->Next;   \
        free(*_head_ptr_ptr_try_pop_front);                         \
        *_head_ptr_ptr_try_pop_front = _next;                       \
        _ok = true;                                                 \
    }                                                               \
    _ok;                                                            \
})

#define List_PushBack(HeadPtrPtr, TailPtrPtr, Val)          \
do {                                                        \
    __auto_type _head_ptr_ptr_push_back = (HeadPtrPtr);     \
    if (NULL == *_head_ptr_ptr_push_back) {                 \
        List_PushFront(_head_ptr_ptr_push_back, (Val));     \
        *(TailPtrPtr) = *_head_ptr_ptr_push_back;           \
    } else {                                                \
        typeof(*_head_ptr_ptr_push_back) _newTail = NULL;   \
        List_PushFront(&_newTail, (Val));                   \
        (*(TailPtrPtr))->Next = _newTail;                   \
        *(TailPtrPtr) = _newTail;                           \
    }                                                       \
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
