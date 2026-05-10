#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "map.h"

#include "testing/testing.h"

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

size_t IntHashIdentity(int value) {
    return (size_t) value;
}

size_t IntHashConst(int unused) {
    (void) unused;
    return 42;
}

bool IntEquals(int a, int b) { return a == b; }

Testing_Fact(Put_associates_key_with_value) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    int const key = 42;
    int const value = 1337;

    Map_Put(&sut, key, value);

    size_t usedEntriesCount = 0;
    for (size_t i = 0; i < sut.Capacity; i++) {
        __auto_type entry = sut.Entries[i];
        if (false == entry.Used) { continue; }

        usedEntriesCount++;
        Testing_Assert(key == entry.Key, "expected key to be %d but was %d", key, entry.Key);
        Testing_Assert(value == entry.Value, "expected value to be %d but was %d", key, entry.Key);
    }
    Testing_Assert(1 == usedEntriesCount, "expected one entry to be used");

    Map_Free(&sut);
}

Testing_Fact(Put_overwrites_values_if_called_with_same_key) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    int const key = 42;
    int const value = 1337;
    int const newValue = 69;

    Map_Put(&sut, key, value);
    Map_Put(&sut, key, newValue);

    size_t usedEntriesCount = 0;
    for (size_t i = 0; i < sut.Capacity; i++) {
        __auto_type entry = sut.Entries[i];
        if (false == entry.Used) { continue; }

        usedEntriesCount++;
        Testing_Assert(key == entry.Key, "expected key to be %d but was %d", key, entry.Key);
        Testing_Assert(newValue == entry.Value, "expected value to be %d but was %d", key, entry.Key);
    }
    Testing_Assert(1 == usedEntriesCount, "expected one entry to be used");

    Map_Free(&sut);
}

Testing_Fact(Put_only_updates_key_when_it_is_first_inserted) {
    Map_Of(char const *, int) sut = {.Hash = StrHash, .KeyEquals = StrEquals};

    char const *key = "Hello";
    char const *newKey = strdup(key);
    int const value = 1337;
    int const newValue = 69;

    Map_Put(&sut, key, value);
    Map_Put(&sut, newKey, newValue);

    Testing_Assert(key != newKey, "keys must be distinct objects");
    Testing_Assert(StrEquals(key, newKey), "keys must be equal strings");

    size_t usedEntriesCount = 0;
    for (size_t i = 0; i < sut.Capacity; i++) {
        __auto_type entry = sut.Entries[i];
        if (false == entry.Used) { continue; }

        usedEntriesCount++;
        Testing_Assert(key == entry.Key, "expected key to be \"%s\" but was \"%s\"", key, entry.Key);
        Testing_Assert(newValue == entry.Value, "expected value to be \"%s\" but was \"%s\"", key, entry.Key);
    }
    Testing_Assert(1 == usedEntriesCount, "expected one entry to be used");

    free((void *) newKey);
    Map_Free(&sut);
}

Testing_Fact(Put_handles_collisions_with_constant_hash) {
    Map_Of(int, int) sut = {.Hash = IntHashConst, .KeyEquals = IntEquals};

    size_t const expectedSize = 42;
    for (size_t i = 0; i < expectedSize; i++) {
        Map_Put(&sut, i * i, i);
    }

    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    Map_Free(&sut);
}

Testing_Fact(Size_is_equal_to_number_of_distinct_keys_inserted) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    size_t const expectedSize = 42;
    for (size_t i = 0; i < expectedSize; i++) {
        Map_Put(&sut, i * i, i);
    }

    Testing_Assert(expectedSize == sut.Size, "expected size to be %zu but was %zu", expectedSize, sut.Size);

    Map_Free(&sut);
}

Testing_Fact(Size_is_not_increased_when_Put_is_called_with_existing_key) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    for (size_t i = 0; i < 100; i++) {
        Map_Put(&sut, 42, i);
    }

    Testing_Assert(1 == sut.Size, "expected size to be 1 but was %zu", sut.Size);

    Map_Free(&sut);
}

Testing_Fact(At_returns_NULL_if_key_does_not_exist) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    Map_Put(&sut, 1, 1);
    Map_Put(&sut, 2, 4);
    Map_Put(&sut, 3, 9);

    Testing_Assert(NULL == Map_At(sut, 4), "expected At to return NULL");

    Map_Free(&sut);
}

Testing_Fact(At_returns_pointer_to_value_for_existing_key) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    Map_Put(&sut, 1, 1);
    Map_Put(&sut, 2, 4);
    Map_Put(&sut, 3, 9);

    int *valuePtr = Map_At(sut, 2);
    Testing_Assert(NULL != valuePtr, "expected At to return non-NULL pointer");

    *valuePtr = 42;

    Testing_Assert(42 == Map_GetOrDefault(sut, 2, -1), "expected value to be updated via pointer");

    Map_Free(&sut);
}

Testing_Fact(TryGet_returns_false_for_empty_map) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    int value;
    Testing_Assert(false == Map_TryGet(sut, 42, &value), "expected TryGet to return false for empty map");

    Map_Free(&sut);
}

Testing_Fact(TryGet_returns_false_when_key_does_not_exist) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    size_t const expectedSize = 42;
    for (size_t i = 0; i < expectedSize; i++) {
        Map_Put(&sut, i * i, i);
    }

    int value;
    Testing_Assert(
            false == Map_TryGet(sut, expectedSize * expectedSize, &value),
            "expected TryGet to return false for key %d", (int) (expectedSize * expectedSize)
    );

    Map_Free(&sut);
}

Testing_Fact(TryGet_returns_false_when_hash_exists_but_key_does_not) {
    Map_Of(int, int) sut = {.Hash = IntHashConst, .KeyEquals = IntEquals};

    Map_Put(&sut, 10, 0);

    int const key = 20;
    int value;
    Testing_Assert(false == Map_TryGet(sut, key, &value), "expected TryGet to return false for key %d", key);

    Map_Free(&sut);
}

Testing_Fact(TryGet_retrieves_all_inserted_values_with_distinct_keys) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    int const keys[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    int const values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    size_t const keysCount = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < keysCount; i++) {
        Map_Put(&sut, keys[i], values[i]);
    }

    for (size_t i = 0; i < keysCount; i++) {
        int value;
        Testing_Assert(true == Map_TryGet(sut, keys[i], &value), "expected TryGet to return true for inserted key");
        Testing_Assert(values[i] == value, "expected value at key=%d to be %d but was %d", keys[i], values[i], value);
    }

    Map_Free(&sut);
}

Testing_Fact(GetOrDefault_returns_value_if_key_exists) {
    Map_Of(char const *, int) sut = {.Hash = StrHash, .KeyEquals = StrEquals};

    char const *const key = "Hello";
    int const value = 42;
    Map_Put(&sut, key, value);

    Testing_Assert(value == Map_GetOrDefault(sut, key, -1), "expected GetOrDefault to return existing value");

    Map_Free(&sut);
}

Testing_Fact(GetOrDefault_returns_default_if_key_does_not_exist) {
    Map_Of(char const *, int) sut = {.Hash = StrHash, .KeyEquals = StrEquals};

    char const *const key = "Hello";
    int const value = 42;
    Map_Put(&sut, key, value);

    Testing_Assert(-1 == Map_GetOrDefault(sut, key + 1, -1), "expected GetOrDefault to return default value");

    Map_Free(&sut);
}

Testing_Fact(ForEach_never_executes_body_for_empty_list) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    int bodyExecuted = false;
    Map_ForEach(pValue, sut) {
        (void) pValue;
        bodyExecuted = true;
    }

    Testing_Assert(false == bodyExecuted, "body must not be executed for empty map");

    Map_Free(&sut);
}

Testing_Fact(ForEach_iterates_over_all_elements) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};

    int const keys[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    int const values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    size_t const keysCount = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < keysCount; i++) {
        Map_Put(&sut, keys[i], values[i]);
    }

    bool visited[keysCount];
    memset(visited, 0x00, sizeof(visited));
    Map_ForEach(entry, sut) {
        size_t keyIndex = 0;
        while (keyIndex < keysCount && keys[keyIndex] != entry->Key) { keyIndex++; }

        Testing_Assert(keyIndex < keysCount, "expected key to be one of inserted keys");
        if (keyIndex >= keysCount) break;

        Testing_Assert(values[keyIndex] == entry->Value, "wrong value");
        Testing_Assert(false == visited[keyIndex], "expected key to not be visited already");

        visited[keyIndex] = true;
    }

    size_t visitedCount = 0;
    for (size_t i = 0; i < keysCount; i++) {
        visitedCount += visited[i];
    }

    Testing_Assert(keysCount == visitedCount, "expected ForEach to iterate over all inserted keys");


    Map_Free(&sut);
}

Testing_Fact(Empty_returns_true_for_empty_map) {
    Map_Of(int, int) sut = {0};

    Testing_Assert(Map_Empty(sut), "expected Empty to return true");

    Map_Free(&sut);
}

Testing_Fact(Empty_returns_false_for_non_empty_map) {
    Map_Of(int, int) sut = {.Hash = IntHashIdentity, .KeyEquals = IntEquals};
    Map_Put(&sut, 1, 2);

    Testing_Assert(false == Map_Empty(sut), "expected Empty to return false");

    Map_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(Put_associates_key_with_value),
        Testing_AddTest(Put_handles_collisions_with_constant_hash),
        Testing_AddTest(Put_overwrites_values_if_called_with_same_key),
        Testing_AddTest(Put_only_updates_key_when_it_is_first_inserted),
        Testing_AddTest(Size_is_equal_to_number_of_distinct_keys_inserted),
        Testing_AddTest(Size_is_not_increased_when_Put_is_called_with_existing_key),
        Testing_AddTest(TryGet_returns_false_for_empty_map),
        Testing_AddTest(At_returns_NULL_if_key_does_not_exist),
        Testing_AddTest(At_returns_pointer_to_value_for_existing_key),
        Testing_AddTest(TryGet_returns_false_when_key_does_not_exist),
        Testing_AddTest(TryGet_returns_false_when_hash_exists_but_key_does_not),
        Testing_AddTest(TryGet_retrieves_all_inserted_values_with_distinct_keys),
        Testing_AddTest(GetOrDefault_returns_value_if_key_exists),
        Testing_AddTest(GetOrDefault_returns_default_if_key_does_not_exist),
        Testing_AddTest(ForEach_never_executes_body_for_empty_list),
        Testing_AddTest(ForEach_iterates_over_all_elements),
        Testing_AddTest(Empty_returns_true_for_empty_map),
        Testing_AddTest(Empty_returns_false_for_non_empty_map),
};

Testing_RunAllTests();