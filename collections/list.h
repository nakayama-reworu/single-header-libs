#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>

#define LIST__CallChecked(Callee, ArgsList) \
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

#define LIST__Concat_(A, B)   A ## B
#define LIST__Concat(A, B)    LIST__Concat_(A, B)

#define List(TValue)                            \
struct LIST__Concat(List_, __LINE__) {          \
    TValue Value;                               \
    struct LIST__Concat(List_, __LINE__) *Next; \
}

#define List_Empty(ListType) ((ListType *) NULL)

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
    __auto_type _headPtrPtr_pushFront = (HeadPtrPtr);       \
    __auto_type _v = (Val);                                 \
    typeof(*_headPtrPtr_pushFront) _newHead =               \
        LIST__CallChecked(calloc, (1, sizeof(*_newHead)));  \
    *_newHead = (typeof(*_newHead)) {                       \
        .Value = _v,                                        \
        .Next = *_headPtrPtr_pushFront                      \
    };                                                      \
    *_headPtrPtr_pushFront = _newHead;                      \
} while (0)

#define List_TryPopFront(HeadPtrPtr, ValuePtr)                  \
({                                                              \
    __auto_type _headPtrPtr_tryPopFront = (HeadPtrPtr);         \
    bool _ok = false;                                           \
    if (NULL != *_headPtrPtr_tryPopFront) {                     \
        *(ValuePtr) = (*_headPtrPtr_tryPopFront)->Value;        \
        __auto_type _next = (*_headPtrPtr_tryPopFront)->Next;   \
        free(*_headPtrPtr_tryPopFront);                         \
        *_headPtrPtr_tryPopFront = _next;                       \
        _ok = true;                                             \
    }                                                           \
    _ok;                                                        \
})

#define List_PushBack(HeadPtrPtr, TailPtrPtr, Val)          \
do {                                                        \
    __auto_type _headPtrPtr_pushBack = (HeadPtrPtr);        \
    if (NULL == *_headPtrPtr_pushBack) {                    \
        List_PushFront(_headPtrPtr_pushBack, (Val));        \
        *(TailPtrPtr) = *_headPtrPtr_pushBack;              \
    } else {                                                \
        typeof(*_headPtrPtr_pushBack) _newTail = NULL;      \
        List_PushFront(&_newTail, (Val));                   \
        (*(TailPtrPtr))->Next = _newTail;                   \
        *(TailPtrPtr) = _newTail;                           \
    }                                                       \
} while (0)

#define LIST__TypeofMember(Type, Member)    typeof(((Type) {}).Member)

#define List_ForEach(ValuePtr, HeadPtr)                                             \
__auto_type LIST__Concat(_it_, __LINE__) = (HeadPtr);                               \
for (                                                                               \
    LIST__TypeofMember(typeof(*LIST__Concat(_it_, __LINE__)), Value) *ValuePtr =    \
        NULL != LIST__Concat(_it_, __LINE__)                                        \
            ? &LIST__Concat(_it_, __LINE__)->Value                                  \
            : NULL;                                                                 \
    NULL != LIST__Concat(_it_, __LINE__);                                           \
    LIST__Concat(_it_, __LINE__) = LIST__Concat(_it_, __LINE__)->Next,              \
    ValuePtr =                                                                      \
        NULL != LIST__Concat(_it_, __LINE__)                                        \
            ? &LIST__Concat(_it_, __LINE__)->Value                                  \
            : NULL                                                                  \
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

#define List_IsEmpty(HeadPtr) (NULL == (HeadPtr))

#endif // LIST_H
