#include "dynarray/dynarray.h"
#include "hashtable/hashtable.h"

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>


DeclarePair(int, int);


uint64_t IntHash(const void *p) {
    return *(int *) p;
}


void PrintIntIntPair(void *p) {
    Pair(int, int) *pair = p;
    printf("Pair{.Key=%d, .Value=%d}\n", pair->Key, pair->Value);
}


int CompareByValueDescending(const void *addr1, const void *addr2) {
    const Pair(int, int) *p1 = addr1, *p2 = addr2;

    return p2->Value - p1->Value;
}


int main(void) {
    srand(0); // NOLINT
    int *numbers = ARRAY_EMPTY(typeof(*numbers));
    for (size_t i = 0; i < 100; i++) {
        ARRAY_APPEND(numbers, rand() % 16); // NOLINT
    }

    Pair(int, int) **frequencies = HashTable(typeof(**frequencies), IntHash, memcmp);
    for (int *n = numbers; n != ARRAY_END(numbers); n++) {
        int *count;
        if (NULL == (count = HashTable_At(frequencies, *n))) {
            count = HashTable_Put(frequencies, *n, 1);
        } else {
            *count = *count + 1;
        }
    }

    Pair(int, int) *pairs = ARRAY_EMPTY(typeof(*pairs));

    for (HashTableIterator it = HashTable_BeginEntries(frequencies);
         HashTable_HasMoreEntries(it);
         HashTable_NextEntry(&it)) {
        ARRAY_APPEND(pairs, HashTable_Entry(frequencies, it));
    }

    qsort(pairs, ArraySize(pairs), sizeof(*pairs), CompareByValueDescending);

    ARRAY_FOREACH_ELEMENT(pairs, PrintIntIntPair);

    HashTable_PrintStats(frequencies);
    HashTable_Free(frequencies, NULL);
    ARRAY_FREE(pairs);
    ARRAY_FREE(numbers);

    return EXIT_SUCCESS;
}