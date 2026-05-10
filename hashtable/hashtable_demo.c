#include "dynarray/dynarray.h"
#include "hashtable/hashtable.h"

#include <stdlib.h>
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
    int *numbers = Array_EmptyOfType(typeof(*numbers));
    for (size_t i = 0; i < 100; i++) {
        Array_Append(numbers, rand() % 16); // NOLINT
    }

    Pair(int, int) **frequencies = HashTable(typeof(**frequencies), IntHash, memcmp);
    Array_ForEach(n, numbers)  {
        int *count;
        if (NULL == (count = HashTable_At(frequencies, n))) {
            count = HashTable_Put(frequencies, n, 1);
        } else {
            *count = *count + 1;
        }
    }

    Pair(int, int) *pairs = Array_EmptyOfType(typeof(*pairs));

    size_t total_count = 0;
    HashTable_ForEach(pair, frequencies) {
        Array_Append(pairs, pair);
    }
    HashTable_ForEach(pair, frequencies) {
        total_count += pair.Value;
    }
    assert(total_count == Array_Size(numbers));

    qsort(pairs, Array_Size(pairs), sizeof(*pairs), CompareByValueDescending);

    Array_ForEachElement(pairs, PrintIntIntPair);

    HashTable_PrintStats(frequencies);

    HashTable_Free(frequencies, NULL);

    Array_Free(pairs, NULL);
    Array_Free(numbers, NULL);

    return EXIT_SUCCESS;
}