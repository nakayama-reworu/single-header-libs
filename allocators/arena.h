#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdalign.h>

// TODO limit size ana alignment to reasonable values to avoid possible overflow when calculating aligned addresses

typedef struct ArenaRegion ArenaRegion;

typedef struct ArenaAllocator ArenaAllocator;

struct ArenaAllocator {
    ArenaRegion *Region;
};

#define Arena_Empty()   ((ArenaAllocator) {0})

void *Arena_Allocate(ArenaAllocator allocator[static 1], size_t size, size_t alignment);

void Arena_Free(ArenaAllocator allocator[static 1]);

#define Arena_New(ArenaPtr, Type) (Type *) Arena_Allocate((ArenaPtr), sizeof(Type), alignof(Type));

#define Arena_NewArray(ArenaPtr, Type, Count) (Type *) Arena_Allocate((ArenaPtr), (Count) * sizeof(Type), alignof(Type))

#define Arena_Copy(ArenaPtr, Src, Count)    \
({                                          \
    size_t _count = (Count);                \
    void *_p = Arena_Allocate(              \
        (ArenaPtr),                         \
        _count * sizeof(uint8_t),           \
        alignof(uint8_t)                    \
    );                                      \
    memcpy(_p, (Src), _count);              \
    (typeof(Src)) _p;                       \
})


