#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "call_checked.h"

#ifndef CallChecked
#error 'CallChecked' is required by this implementation
#endif

#ifndef CONCAT
#define CONCAT_(A, B)   A ## B
#define CONCAT(A, B)    CONCAT_(A, B)
#endif

size_t StrHash(char const *s) {
    unsigned long hash = 5381;
    int c;

    while ('\0' != (c = (unsigned char) *s++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

bool StrEquals(char const *s1, char const *s2) {
    return 0 == strcmp(s1, s2);
}

#define Map_Of(TKey, TValue)        \
struct {                            \
    size_t Size;                    \
    size_t Capacity;                \
    size_t (*Hash)(TKey);           \
    bool (*KeyEquals)(TKey, TKey);  \
    struct {                        \
        TKey Key;                   \
        TValue Value;               \
        bool Used;                  \
    } *Items;                       \
}

#define Map_Free(MapPtr)                    \
do {                                        \
    free((MapPtr)->Items);                  \
    *(MapPtr) = (typeof(*(MapPtr))) { 0 };  \
} while (0)

#define Map_FindSlot(MapPtr, Key_)                                                  \
({                                                                                  \
    typeof(*((MapPtr)->Items)) *_found_slot = NULL;                                 \
    if ((MapPtr)->Capacity > 0) {                                                   \
        size_t _index = (MapPtr)->Hash(Key_) % (MapPtr)->Capacity;                  \
        for (size_t _i_fs = 0; _i_fs < (MapPtr)->Capacity; _i_fs++) {               \
            __auto_type _item = (MapPtr)->Items[_index];                            \
            if (false == _item.Used || (MapPtr)->KeyEquals((Key_), _item.Key)) {    \
                break;                                                              \
            }                                                                       \
            _index = (_index + 1) % (MapPtr)->Capacity;                             \
        }                                                                           \
        _found_slot = &((MapPtr)->Items[_index]);                                   \
    }                                                                               \
    _found_slot;                                                                    \
})

#define Map_Reserve(MapPtr, NewCapacity)                        \
do {                                                            \
    size_t const _oldCapacity = (MapPtr)->Capacity;             \
    size_t const _newCapacity = (NewCapacity);                  \
    if (_oldCapacity >= _newCapacity) { break; }                \
                                                                \
    __auto_type _oldItems = (MapPtr)->Items;                    \
    (MapPtr)->Items = CallChecked(                              \
        calloc, (_newCapacity, sizeof(*((MapPtr)->Items)))      \
    );                                                          \
    (MapPtr)->Capacity = _newCapacity;                          \
                                                                \
    for (size_t _i = 0; _i < _oldCapacity; _i++) {              \
        __auto_type _e = _oldItems[_i];                         \
        if (false == _e.Used) {                                 \
            continue;                                           \
        }                                                       \
        __auto_type _slot = Map_FindSlot((MapPtr), _e.Key);     \
        *_slot = _e;                                            \
    }                                                           \
                                                                \
    (MapPtr)->Capacity = _newCapacity;                          \
    free(_oldItems);                                            \
} while (0)

#define Map_Put(MapPtr, Key_, Value_)                               \
({                                                                  \
    if (3 * ((MapPtr)->Size + 1) >= 2 * (MapPtr)->Capacity) {       \
        Map_Reserve((MapPtr), 3 * (MapPtr)->Capacity / 2 + 3);      \
    }                                                               \
    __auto_type _slot = Map_FindSlot((MapPtr), (Key_));             \
    _slot->Value = (Value_);                                        \
    if (false == _slot->Used) {                                     \
        _slot->Key = (Key_);                                        \
        _slot->Used = true;                                         \
        (MapPtr)->Size += 1;                                        \
    }                                                               \
    &(_slot->Value);                                                \
})

#define Map_TryGet(MapPtr, Key_, ValuePtr)                  \
({                                                          \
    __auto_type _slot = Map_FindSlot((MapPtr), (Key_));     \
    bool _ok = false;                                       \
    if (NULL != _slot && _slot->Used) {                     \
        *ValuePtr = _slot->Value;                           \
        _ok = true;                                         \
    }                                                       \
    _ok;                                                    \
})

#define Map_GetOrDefault(MapPtr, Key_, DefaultExpr)         \
({                                                          \
    typeof((MapPtr)->Items->Value) _value;                  \
    if (false == Map_TryGet((MapPtr), (Key_), &_value)) {   \
        _value = (DefaultExpr);                             \
    }                                                       \
    _value;                                                 \
})

#define Map_TryFindNextUsedIndex(MapPtr, BaseIndex, NextIndexPtr)   \
({                                                                  \
    size_t _i_next = (BaseIndex);                                   \
    bool _ok = false;                                               \
    for (; _i_next < (MapPtr)->Capacity; _i_next++) {               \
        if (false == (MapPtr)->Items[_i_next].Used) {               \
            continue;                                               \
        }                                                           \
        *(NextIndexPtr) = _i_next;                                  \
        _ok = true;                                                 \
        break;                                                      \
    }                                                               \
     _ok;                                                           \
})

#define Map_ForEach(EntryPtr, MapPtr)                                                       \
size_t CONCAT(_i_, __LINE__) = 0;                                                           \
for (                                                                                       \
    typeof(*((MapPtr)->Items)) *pEntry =                                                    \
        Map_TryFindNextUsedIndex(&map, CONCAT(_i_, __LINE__), &CONCAT(_i_, __LINE__))       \
            ? &((MapPtr)->Items[CONCAT(_i_, __LINE__)])                                     \
            : NULL;                                                                         \
    NULL != pEntry;                                                                         \
    pEntry =                                                                                \
        Map_TryFindNextUsedIndex(&map, CONCAT(_i_, __LINE__) + 1, &CONCAT(_i_, __LINE__))   \
            ? &((MapPtr)->Items[CONCAT(_i_, __LINE__)])                                     \
            : NULL                                                                          \
)

int main(void) {
    __auto_type map = (Map_Of(char const *, int)) {.Hash = StrHash, .KeyEquals = StrEquals};

    char const *originalStr = "A quick brown fox jumps over the lazy dog A quick brown fox jumps over the lazy dog  A quick brown fox";
    char s[1024] = {0};
    memcpy(s, originalStr, strlen(originalStr) + 1);
    char *savePtr;
    char *token = strtok_r(s, " ", &savePtr);
    while (NULL != token) {
        int count;
        if (Map_TryGet(&map, token, &count)) {
            Map_Put(&map, token, count + 1);
        } else {
            Map_Put(&map, strdup(token), 1);
        }

        token = strtok_r(NULL, " ", &savePtr);
    }

    Map_ForEach(pEntry, &map) {
        printf("[ForEach] key=%s, value=%d\n", pEntry->Key, pEntry->Value);
    }
    Map_ForEach(pEntry, &map) { free((void *) pEntry->Key); }

    printf("%zu %zu %p\n", map.Size, map.Capacity, map.Items);
    Map_Free(&map);
    printf("%zu %zu %p\n", map.Size, map.Capacity, map.Items);

    return EXIT_SUCCESS;
}