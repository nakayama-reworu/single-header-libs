#include "vector.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

int main(void) {
    int *a = vector_empty_of_type(int);
    if (NULL == a) {
        exit(EXIT_FAILURE);
    }

    vector_append(a, 1);
    vector_append(a, 2);

    vector_append_range(a, ((int[]) {3, 4}), 2);
    vector_append_values(a, 5, 6);

    for (size_t i = 0; i < vector_size(a); i++) {
        a[i] = a[i] * a[i];
    }

    vector_foreach(it, a) {
        *it = -*it;
    }

    vector_foreach(it, (const int *) a) {
        printf("%d ", *it);
    }
    printf("\n");

    while (false == vector_empty(a)) {
        printf("%d ", vector_pop(a));
    }
    printf("\n");

    vector_free(a);

    return EXIT_SUCCESS;
}
