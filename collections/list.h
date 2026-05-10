#pragma once

#ifndef CallChecked
#error Please include call_checked.h
#endif

#define LIST_EMPTY  (NULL)

#ifndef CONCAT
#define CONCAT_(A, B)   A ## B
#define CONCAT(A, B)    CONCAT_(A, B)
#endif

#define List_Of(TValue) struct CONCAT(List_, __LINE__) { TValue Value; struct CONCAT(List_, __LINE__) *Next; }

#define List_Free(HeadPtr)          \
do {                                \
    __auto_type _next = (HeadPtr);  \
    __auto_type _it = _next;        \
    while (NULL != _it) {           \
        _next = _it->Next;          \
        free(_it);                  \
        _it = _next;                \
    }                               \
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

#define TYPEOF_MEMBER(Type, Member)    typeof(((Type) {}).Member)

#define List_ForEach(ValuePtr, HeadPtr)                                 \
__auto_type CONCAT(_it_, __LINE__) = (HeadPtr);                         \
for (                                                                   \
    TYPEOF_MEMBER(typeof(*CONCAT(_it_, __LINE__)), Value) *ValuePtr =   \
        NULL != CONCAT(_it_, __LINE__)                                  \
            ? &CONCAT(_it_, __LINE__)->Value                            \
            : NULL;                                                     \
    NULL != CONCAT(_it_, __LINE__);                                     \
    CONCAT(_it_, __LINE__) = CONCAT(_it_, __LINE__)->Next,              \
    ValuePtr =                                                          \
        NULL != CONCAT(_it_, __LINE__)                                  \
            ? &CONCAT(_it_, __LINE__)->Value                            \
            : NULL                                                      \
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

