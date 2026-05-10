#include "hashtable/hashtable.h"

#include <stdlib.h>
#include <stddef.h>


DeclarePair(int, int);


size_t int_hash(const void *p) {
    // https://stackoverflow.com/a/12996028/17654649
    int x = *(int *) p;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

int int_cmp(const void *p1, const void *p2) {
    int *i1 = (int *) p1, *i2 = (int *) p2;
    return *i1 - *i2;
}


void int_int_pair_print(Pair(int, int) pair) {
    printf("Pair{.Key=%d, .Value=%d}\n", pair.Key, pair.Value);
}


int main(void) {
    Pair(int, int) *t = hashtable(typeof(*t), int_hash, int_cmp);

    srand(0); // NOLINT
    const size_t total_numbers = 1000;
    const int max = 100;

    for (size_t i = 0; i < total_numbers; i++) {
        const int n = rand() % max; // NOLINT
        int *count = hashtable_at(t, n);
        if (NULL == count) {
            hashtable_put(t, n, 1);
        } else {
            *count += 1;
        }
    }

    printf("size=%d\n", (int) hashtable_size(t));
    size_t n = 0;
    hashtable_foreach(p, t) {
        n += p.Value;
    }
    hashtable_foreach(p, t) {
        int_int_pair_print(p);
    }
    if (total_numbers != n) {
        LOG_ERROR("Total count is different from expected");
    }

    hashtable_free(t, NULL);

    return EXIT_SUCCESS;
}