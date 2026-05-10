#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAP__CallChecked(Callee, ArgsList)  \
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

#define MAP__Concat_(A, B)   A ## B
#define MAP__Concat(A, B)    MAP__Concat_(A, B)

#define MAP__ArrayLength(Array)         (sizeof(Array) / sizeof(*(Array)))

#define Map(TKey, TValue)           \
struct {                            \
    size_t Size;                    \
    size_t Capacity;                \
    size_t (*Hash)(TKey);           \
    bool (*KeyEquals)(TKey, TKey);  \
    struct {                        \
        TKey Key;                   \
        TValue Value;               \
        bool Used;                  \
    } *Entries;                     \
}

#define Map_Empty(MapType, Hash_, KeyEquals_) ((MapType) {.Hash = (Hash_), .KeyEquals = (KeyEquals_)})

#define MAP__WithEntries(Map_, ...)                                             \
({                                                                              \
    __auto_type _map_withEntries = (Map_);                                      \
    typeof(_map_withEntries.Entries[0]) _newEntries[] = {__VA_ARGS__};          \
    size_t const _newEntriesCount = MAP__ArrayLength(_newEntries);              \
    for (                                                                       \
        size_t _i_withEntries = 0;                                              \
        _i_withEntries < _newEntriesCount;                                      \
        _i_withEntries++                                                        \
    ) {                                                                         \
        Map_Put(                                                                \
            &_map_withEntries,                                                  \
            _newEntries[_i_withEntries].Key,                                    \
            _newEntries[_i_withEntries].Value);                                 \
    }                                                                           \
    _map_withEntries;                                                           \
})

#define Map_Of(MapType, Hash_, KeyEquals_, ...) MAP__WithEntries(Map_Empty(MapType, Hash_, KeyEquals_), ##__VA_ARGS__)

#define Map_Free(MapPtr)                    \
do {                                        \
    free((MapPtr)->Entries);                \
    *(MapPtr) = (typeof(*(MapPtr))) { 0 };  \
} while (0)

#define MAP__FindSlot(Map_, Key_)                                                       \
({                                                                                      \
    __auto_type _map_find_slot = (Map_);                                                \
    typeof(*(_map_find_slot.Entries)) *_found_slot = NULL;                              \
    if (_map_find_slot.Capacity > 0) {                                                  \
        size_t _index = _map_find_slot.Hash(Key_) % _map_find_slot.Capacity;            \
        for (size_t _i_fs = 0; _i_fs < _map_find_slot.Capacity; _i_fs++) {              \
            __auto_type _item = _map_find_slot.Entries[_index];                         \
            if (false == _item.Used || _map_find_slot.KeyEquals((Key_), _item.Key)) {   \
                break;                                                                  \
            }                                                                           \
            _index = (_index + 1) % _map_find_slot.Capacity;                            \
        }                                                                               \
        _found_slot = &(_map_find_slot.Entries[_index]);                                \
    }                                                                                   \
    _found_slot;                                                                        \
})

#define MAP__Reserve(MapPtr, NewCapacity)                               \
do {                                                                    \
    __auto_type _map_ptr_reserve = (MapPtr);                            \
    size_t const _oldCapacity = _map_ptr_reserve->Capacity;             \
    size_t const _newCapacity = (NewCapacity);                          \
    if (_oldCapacity >= _newCapacity) { break; }                        \
                                                                        \
    __auto_type _oldEntries = _map_ptr_reserve->Entries;                \
    _map_ptr_reserve->Entries = MAP__CallChecked(                       \
        calloc, (_newCapacity, sizeof(*(_map_ptr_reserve->Entries)))    \
    );                                                                  \
    _map_ptr_reserve->Capacity = _newCapacity;                          \
                                                                        \
    for (size_t _i = 0; _i < _oldCapacity; _i++) {                      \
        __auto_type _e = _oldEntries[_i];                               \
        if (false == _e.Used) {                                         \
            continue;                                                   \
        }                                                               \
        __auto_type _slot = MAP__FindSlot(*_map_ptr_reserve, _e.Key);   \
        *_slot = _e;                                                    \
    }                                                                   \
                                                                        \
    _map_ptr_reserve->Capacity = _newCapacity;                          \
    free(_oldEntries);                                                  \
} while (0)

#define Map_Put(MapPtr, Key_, Value_)                                   \
({                                                                      \
    __auto_type _map_ptr_put = (MapPtr);                                \
    if (3 * (_map_ptr_put->Size + 1) >= 2 * _map_ptr_put->Capacity) {   \
        MAP__Reserve(_map_ptr_put, 3 * _map_ptr_put->Capacity / 2 + 3); \
    }                                                                   \
    __auto_type _slot_put = MAP__FindSlot(*_map_ptr_put, (Key_));       \
    _slot_put->Value = (Value_);                                        \
    if (false == _slot_put->Used) {                                     \
        _slot_put->Key = (Key_);                                        \
        _slot_put->Used = true;                                         \
        _map_ptr_put->Size += 1;                                        \
    }                                                                   \
    &(_slot_put->Value);                                                \
})

#define Map_At(Map_, Key_)                                  \
({                                                          \
    __auto_type _slot_at = MAP__FindSlot((Map_), (Key_));   \
    (NULL == _slot_at || false == _slot_at->Used            \
        ? NULL :                                            \
        &_slot_at->Value);                                  \
})

#define Map_TryGet(Map_, Key_, ValuePtr)                    \
({                                                          \
    __auto_type _value_try_get = Map_At((Map_), (Key_));    \
    if (NULL != _value_try_get) {                           \
        *ValuePtr = *_value_try_get;                        \
    }                                                       \
    NULL != _value_try_get;                                 \
})

#define Map_GetOrDefault(Map_, Key_, DefaultExpr)                   \
({                                                                  \
    typeof((Map_).Entries->Value) _value_or_default;                \
    if (false == Map_TryGet((Map_), (Key_), &_value_or_default)) {  \
        _value_or_default = (DefaultExpr);                          \
    }                                                               \
    _value_or_default;                                              \
})

#define MAP__TryFindNextUsedIndex(Map_, BaseIndex, NextIndexPtr)        \
({                                                                      \
    __auto_type _map_try_find_next_index = (Map_);                      \
    size_t _i_next = (BaseIndex);                                       \
    bool _ok = false;                                                   \
    for (; _i_next < _map_try_find_next_index.Capacity; _i_next++) {    \
        if (false == _map_try_find_next_index.Entries[_i_next].Used) {  \
            continue;                                                   \
        }                                                               \
        *(NextIndexPtr) = _i_next;                                      \
        _ok = true;                                                     \
        break;                                                          \
    }                                                                   \
     _ok;                                                               \
})

#define Map_ForEach(EntryPtr, Map_)                                                                 \
size_t MAP__Concat(_i_, __LINE__) = 0;                                                              \
__auto_type MAP__Concat(_map_for_each_, __LINE__) = (Map_);                                         \
for (                                                                                               \
    typeof(*(MAP__Concat(_map_for_each_, __LINE__).Entries)) *EntryPtr =                            \
        MAP__TryFindNextUsedIndex(                                                                  \
            MAP__Concat(_map_for_each_, __LINE__),                                                  \
            MAP__Concat(_i_, __LINE__),                                                             \
            &MAP__Concat(_i_, __LINE__)                                                             \
        )                                                                                           \
            ? &(MAP__Concat(_map_for_each_, __LINE__).Entries[MAP__Concat(_i_, __LINE__)])          \
            : NULL;                                                                                 \
    NULL != EntryPtr;                                                                               \
    EntryPtr =                                                                                      \
        MAP__TryFindNextUsedIndex(                                                                  \
            MAP__Concat(_map_for_each_, __LINE__),                                                  \
            MAP__Concat(_i_, __LINE__) + 1,                                                         \
            &MAP__Concat(_i_, __LINE__)                                                             \
        )                                                                                           \
            ? &(MAP__Concat(_map_for_each_, __LINE__).Entries[MAP__Concat(_i_, __LINE__)])          \
            : NULL                                                                                  \
)

#define Map_IsEmpty(Map_) (0 == (Map_).Size)

#endif // MAP_H
