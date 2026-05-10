#include "dynarray.h"


typedef struct {
    size_t ElementSizeBytes;
    size_t Size;
    size_t Capacity;
} ArrayHeader;

#define IMPL_ARRAY_HEADER(Array) (((ArrayHeader *) Array) - 1)

#define IMPL_ARRAY_GROW_FACTOR 2

void *Array_New(const size_t size, const size_t element_size_bytes) {
    size_t capacity = size > ARRAY_MIN_CAPACITY ? size : ARRAY_MIN_CAPACITY;

    const size_t total_size_bytes = capacity * element_size_bytes + sizeof(ArrayHeader);

    ArrayHeader *header;
    if (NULL == (header = (ArrayHeader *) malloc(total_size_bytes))) {
        perror(NAME_OF(ArrayNew));
        return NULL;
    }

    *header = (ArrayHeader) {
            .ElementSizeBytes = element_size_bytes,
            .Size = size,
            .Capacity = capacity,
    };

    return SHIFT(header, sizeof(ArrayHeader));
}


void Array_Free(void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return;
    }

    free((void *) IMPL_ARRAY_HEADER(array));
}


size_t Array_ElementSize(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->ElementSizeBytes;
}


size_t Array_Size(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->Size;
}


bool Array_IsEmpty(const void *array) {
    return 0 == Array_Size(array);
}


size_t Array_Capacity(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->Capacity;
}


void *Array_ReserveToFit(void *array, size_t size) {
    if (NULL == array) {
        LOG_NULL(array);
        return array;
    }

    if (Array_Capacity(array) >= size) {
        return array;
    }

    size_t new_capacity = Array_Capacity(array);
    while (new_capacity < size) {
        new_capacity *= IMPL_ARRAY_GROW_FACTOR;
    }

    if (new_capacity < size) {
        LOG_ERROR(
                NAME_OF(new_capacity) "=%d is not enough for " NAME_OF(size) "=%d elements",
                (int) new_capacity, (int) size
        );
        return NULL;
    }
    assert(new_capacity >= size);

    LOG_DEBUG(NAME_OF(new_capacity) "=%d", (int) new_capacity);

    void *new_array;
    if (NULL == (new_array = Array_New(new_capacity, Array_ElementSize(array)))) {
        perror(NAME_OF(ArrayReserveToFit));
        return NULL;
    }

    memcpy(new_array, array, Array_Size(array) * Array_ElementSize(array));
    IMPL_ARRAY_HEADER(new_array)->Size = Array_Size(array);

    Array_Free(array);

    return new_array;
}


void Array_FillFrom(void *array, const void *value) {
    for (size_t i = 0; i < Array_Size(array); i += 1) {
        memcpy(Array_At(array, i), value, Array_ElementSize(array));
    }
}


void *Array_WithAppendedFrom(void *array, const void *element) {
    if (NULL == array) {
        LOG_NULL(array);
        return array;
    }

    const size_t old_size = Array_Size(array);
    const size_t old_capacity = Array_Capacity(array);

    const size_t new_size = old_size + 1;

    if (new_size > old_capacity) {
        if (NULL == (array = Array_ReserveToFit(array, new_size))) {
            LOG_ERROR("Failed to increase array capacity");
            return NULL;
        }
    }

    IMPL_ARRAY_HEADER(array)->Size = new_size;
    memcpy(Array_At(array, old_size), element, Array_ElementSize(array));

    LOG_DEBUG(
            NAME_OF(old_size) "=%d, " NAME_OF(new_size) "=%d, "
            NAME_OF(old_capacity) "=%d, " NAME_OF(new_capacity) "=%d",
            (int) old_size, (int) new_size, (int) old_capacity, (int) Array_Capacity(array)
    );

    return array;
}


void *Array_ExtendWithValues(
        void *array,
        const void *data,
        const size_t elements_count,
        const size_t element_size
) {
    if (Array_ElementSize(array) != element_size) {
        LOG_ERROR(
                NAME_OF(element_size) "=%d is different from array element size (%d)",
                (int) element_size, (int) Array_ElementSize(array)
        );
        return NULL;
    }

    const size_t original_size = Array_Size(array);
    if (NULL == (array = Array_ReserveToFit(array, original_size + elements_count))) {
        LOG_ERROR("Failed to increase capacity");
        return NULL;
    }

    for (size_t i = 0; i < elements_count; i++) {
        array = Array_WithAppendedFrom(array, SHIFT(data, i * element_size));
    }

    assert(Array_Size(array) == original_size + elements_count);

    return array;
}


void *Array_Extend(void *array, const void *other) {
    return Array_ExtendWithValues(array, other, Array_Size(other), Array_ElementSize(other));
}


void *Array_OfValues(
        const void *data,
        const size_t elements_count,
        const size_t element_size
) {
    void *array = Array_New(elements_count, element_size);
    IMPL_ARRAY_HEADER(array)->Size = 0;  // ArrayExtendWithValues will work properly, space already reserved
    return Array_ExtendWithValues(array, data, elements_count, element_size);
}


bool Array_Pop(void *array, void *dst) {
    if (NULL == array) {
        LOG_NULL(array);
        return false;
    }

    if (Array_IsEmpty(array)) {
        LOG_ERROR("Array is empty");
        return false;
    }

    IMPL_ARRAY_HEADER(array)->Size -= 1;

    if (NULL == dst) {
        return true;
    }

    const void *src = Array_At(array, Array_Size(array));
    memcpy(dst, src, Array_ElementSize(array));

    return true;
}
