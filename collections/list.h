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

#define List(TValue)    \
struct {                \
    size_t Size;        \
    struct {            \
        TValue Value;   \
        void *Prev;     \
        void *Next;     \
    } *Head, *Tail;     \
}

#define List_Empty(ListType) ((ListType) {0})

#define List_Free(ListPtr)                                          \
do {                                                                \
    __auto_type _listPtr_free = (ListPtr);                          \
    typeof(_listPtr_free->Head) _cur_free = _listPtr_free->Head;    \
    while (NULL != _cur_free) {                                     \
        typeof(_listPtr_free->Head) _next_free = _cur_free->Next;   \
        free(_cur_free);                                            \
        _cur_free = _next_free;                                     \
    }                                                               \
    *_listPtr_free = List_Empty(typeof(*_listPtr_free));            \
} while (0)

#define LIST__NewNode(NodeType, Val, Next_, Prev_)              \
({                                                              \
    NodeType *const _newNode = (NodeType *) LIST__CallChecked(  \
        calloc, (1, sizeof(*_newNode))                          \
    );                                                          \
    *_newNode = (NodeType) {                                    \
        .Value = (Val),                                         \
        .Next = (Next_),                                        \
        .Prev = (Prev_),                                        \
    };                                                          \
    if (NULL != _newNode->Next) {                               \
        ((NodeType *) _newNode->Next)->Prev = _newNode;         \
    }                                                           \
    if (NULL != _newNode->Prev) {                               \
        ((NodeType *) _newNode->Prev)->Next = _newNode;         \
    }                                                           \
    _newNode;                                                   \
})

#define List_PushFront(ListPtr, Val)                            \
do {                                                            \
    __auto_type _listPtr_pushFront = (ListPtr);                 \
    __auto_type _newNodePtr_pushFront = LIST__NewNode(          \
        typeof(*_listPtr_pushFront->Head),                      \
        (Val),                                                  \
        _listPtr_pushFront->Head,                               \
        NULL                                                    \
    );                                                          \
    _listPtr_pushFront->Head = _newNodePtr_pushFront;           \
    if (NULL == _listPtr_pushFront->Tail) {                     \
        _listPtr_pushFront->Tail = _newNodePtr_pushFront;       \
    }                                                           \
    _listPtr_pushFront->Size++;                                 \
} while (0)

#define List_PushBack(ListPtr, Val)                             \
do {                                                            \
    __auto_type _listPtr_pushBack = (ListPtr);                  \
    __auto_type _newNodePtr_pushBack = LIST__NewNode(           \
        typeof(*_listPtr_pushBack->Head),                       \
        (Val),                                                  \
        NULL,                                                   \
        _listPtr_pushBack->Tail                                 \
    );                                                          \
    _listPtr_pushBack->Tail = _newNodePtr_pushBack;             \
    if (NULL == _listPtr_pushBack->Head) {                      \
        _listPtr_pushBack->Head = _newNodePtr_pushBack;         \
    }                                                           \
    _listPtr_pushBack->Size++;                                  \
} while (0)

#define List_TryPopFront(ListPtr, ValuePtr)                                             \
({                                                                                      \
    __auto_type _listPtr_tryPopFront = (ListPtr);                                       \
    typeof (_listPtr_tryPopFront->Head->Value) *_valuePtr_tryPopFront = (ValuePtr);     \
    typeof(_listPtr_tryPopFront->Head) _head_tryPopFront = _listPtr_tryPopFront->Head;  \
    bool _ok = false;                                                                   \
    if (NULL != _head_tryPopFront) {                                                    \
        if (NULL != _valuePtr_tryPopFront) {                                            \
            *_valuePtr_tryPopFront = _head_tryPopFront->Value;                          \
        }                                                                               \
        if (NULL != _head_tryPopFront->Next) {                                          \
            ((typeof(_head_tryPopFront)) _head_tryPopFront->Next)->Prev = NULL;         \
        } else {                                                                        \
            _listPtr_tryPopFront->Tail = NULL;                                          \
        }                                                                               \
        _listPtr_tryPopFront->Head = _head_tryPopFront->Next;                           \
        _listPtr_tryPopFront->Size--;                                                   \
        free(_head_tryPopFront);                                                        \
        _ok = true;                                                                     \
    }                                                                                   \
    _ok;                                                                                \
})

#define List_TryPopBack(ListPtr, ValuePtr)                                              \
({                                                                                      \
    __auto_type _listPtr_tryPopBack = (ListPtr);                                        \
    typeof (_listPtr_tryPopBack->Head->Value) *_valuePtr_tryPopBack = (ValuePtr);       \
    typeof(_listPtr_tryPopBack->Tail) _tail_tryPopBack = _listPtr_tryPopBack->Tail;     \
    bool _ok = false;                                                                   \
    if (NULL != _tail_tryPopBack) {                                                     \
        if (NULL != _valuePtr_tryPopBack) {                                             \
            *_valuePtr_tryPopBack = _tail_tryPopBack->Value;                            \
        }                                                                               \
        if (NULL != _tail_tryPopBack->Prev) {                                           \
            ((typeof(_tail_tryPopBack)) _tail_tryPopBack->Prev)->Next = NULL;           \
        } else {                                                                        \
            _listPtr_tryPopBack->Head = NULL;                                           \
        }                                                                               \
        _listPtr_tryPopBack->Tail = _tail_tryPopBack->Prev;                             \
        _listPtr_tryPopBack->Size--;                                                    \
        free(_tail_tryPopBack);                                                         \
        _ok = true;                                                                     \
    }                                                                                   \
    _ok;                                                                                \
})

#define LIST__TypeofMember(Type, Member)    typeof(((Type) {}).Member)

#define List_ForEach(ValuePtr, List_)                                               \
__auto_type LIST__Concat(_list_forEach_, __LINE__) = (List_);                       \
__auto_type LIST__Concat(_it_, __LINE__) =                                          \
    LIST__Concat(_list_forEach_, __LINE__).Head;                                    \
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

#define LIST__NthNext(NodePtr, N)                   \
({                                                  \
    long long _n_nthNext = (N);                     \
    __auto_type _nodePtr_nthNext = (NodePtr);       \
    while (_n_nthNext > 0) {                        \
        if (NULL == _nodePtr_nthNext) {             \
            break;                                  \
        }                                           \
        _nodePtr_nthNext = _nodePtr_nthNext->Next;  \
        _n_nthNext--;                               \
    }                                               \
    _nodePtr_nthNext;                               \
})

#define LIST__NthPrev(NodePtr, N)                   \
({                                                  \
    long long _n_nthPrev = (N);                     \
    __auto_type _nodePtr_nthPrev = (NodePtr);       \
    while (_n_nthPrev > 0) {                        \
        if (NULL == _nodePtr_nthPrev) {             \
            break;                                  \
        }                                           \
        _nodePtr_nthPrev = _nodePtr_nthPrev->Prev;  \
        _n_nthPrev--;                               \
    }                                               \
    _nodePtr_nthPrev;                               \
})

#define List_At(List_, Index)                                   \
({                                                              \
    long long _idx_at = (Index);                                \
    __auto_type _list_at = (List_);                             \
    typeof(_list_at.Head) _node_at = NULL;                      \
    if (_idx_at >= 0) {                                         \
        _node_at = LIST__NthNext(_list_at.Head, _idx_at);       \
    } else {                                                    \
        _node_at = LIST__NthPrev(_list_at.Tail, -_idx_at - 1);  \
    }                                                           \
    NULL != _node_at                                            \
        ? &_node_at->Value                                      \
        : NULL;                                                 \
})

#define List_IsEmpty(List_) (0 == (List_).Size)

#endif // LIST_H
