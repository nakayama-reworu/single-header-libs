#pragma once

#include <stdlib.h>
#include <stdbool.h>

#ifndef CallChecked
#error Please include call_checked.h before map.h
#endif

#define MAP_CONCAT_(A, B)   A ## B
#define MAP_CONCAT(A, B)    MAP_CONCAT_(A, B)

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
    } *Entries;                     \
}

#define Map_Free(MapPtr)                    \
do {                                        \
    free((MapPtr)->Entries);                \
    *(MapPtr) = (typeof(*(MapPtr))) { 0 };  \
} while (0)

#define Map_FindSlot(Map, Key_)                                                         \
({                                                                                      \
    __auto_type _map_find_slot = (Map);                                                 \
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

#define Map_Reserve(MapPtr, NewCapacity)                                \
do {                                                                    \
    __auto_type _map_ptr_reserve = (MapPtr);                            \
    size_t const _oldCapacity = _map_ptr_reserve->Capacity;             \
    size_t const _newCapacity = (NewCapacity);                          \
    if (_oldCapacity >= _newCapacity) { break; }                        \
                                                                        \
    __auto_type _oldEntries = _map_ptr_reserve->Entries;                \
    _map_ptr_reserve->Entries = CallChecked(                            \
        calloc, (_newCapacity, sizeof(*(_map_ptr_reserve->Entries)))    \
    );                                                                  \
    _map_ptr_reserve->Capacity = _newCapacity;                          \
                                                                        \
    for (size_t _i = 0; _i < _oldCapacity; _i++) {                      \
        __auto_type _e = _oldEntries[_i];                               \
        if (false == _e.Used) {                                         \
            continue;                                                   \
        }                                                               \
        __auto_type _slot = Map_FindSlot(*_map_ptr_reserve, _e.Key);    \
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
        Map_Reserve(_map_ptr_put, 3 * _map_ptr_put->Capacity / 2 + 3);  \
    }                                                                   \
    __auto_type _slot_put = Map_FindSlot(*_map_ptr_put, (Key_));        \
    _slot_put->Value = (Value_);                                        \
    if (false == _slot_put->Used) {                                     \
        _slot_put->Key = (Key_);                                        \
        _slot_put->Used = true;                                         \
        _map_ptr_put->Size += 1;                                        \
    }                                                                   \
    &(_slot_put->Value);                                                \
})

#define Map_At(Map, Key_)                                   \
({                                                          \
    __auto_type _slot_at = Map_FindSlot((Map), (Key_));     \
    (NULL == _slot_at || false == _slot_at->Used            \
        ? NULL :                                            \
        &_slot_at->Value);                                  \
})

#define Map_TryGet(Map, Key_, ValuePtr)                     \
({                                                          \
    __auto_type _value_try_get = Map_At((Map), (Key_));     \
    if (NULL != _value_try_get) {                           \
        *ValuePtr = *_value_try_get;                        \
    }                                                       \
    NULL != _value_try_get;                                 \
})

#define Map_GetOrDefault(Map, Key_, DefaultExpr)                    \
({                                                                  \
    typeof((Map).Entries->Value) _value_or_default;                 \
    if (false == Map_TryGet((Map), (Key_), &_value_or_default)) {   \
        _value_or_default = (DefaultExpr);                          \
    }                                                               \
    _value_or_default;                                              \
})

#define Map_TryFindNextUsedIndex(Map, BaseIndex, NextIndexPtr)          \
({                                                                      \
    __auto_type _map_try_find_next_index = (Map);                       \
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

#define Map_ForEach(EntryPtr, Map)                                                                  \
size_t MAP_CONCAT(_i_, __LINE__) = 0;                                                               \
__auto_type MAP_CONCAT(_map_for_each_, __LINE__) = (Map);                                           \
for (                                                                                               \
    typeof(*(MAP_CONCAT(_map_for_each_, __LINE__).Entries)) *EntryPtr =                             \
        Map_TryFindNextUsedIndex(                                                                   \
            MAP_CONCAT(_map_for_each_, __LINE__),                                                   \
            MAP_CONCAT(_i_, __LINE__),                                                              \
            &MAP_CONCAT(_i_, __LINE__)                                                              \
        )                                                                                           \
            ? &(MAP_CONCAT(_map_for_each_, __LINE__).Entries[MAP_CONCAT(_i_, __LINE__)])            \
            : NULL;                                                                                 \
    NULL != EntryPtr;                                                                               \
    EntryPtr =                                                                                      \
        Map_TryFindNextUsedIndex(                                                                   \
            MAP_CONCAT(_map_for_each_, __LINE__),                                                   \
            MAP_CONCAT(_i_, __LINE__) + 1,                                                          \
            &MAP_CONCAT(_i_, __LINE__)                                                              \
        )                                                                                           \
            ? &(MAP_CONCAT(_map_for_each_, __LINE__).Entries[MAP_CONCAT(_i_, __LINE__)])            \
            : NULL                                                                                  \
)

#define Map_Empty(Map) (0 == (Map).Size)
