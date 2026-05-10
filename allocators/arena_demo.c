#include <stdio.h>

#include "arena.h"

typedef struct A A;
struct A {
    int Int;
    char const *Str;
};

int main(void) {
    ArenaAllocator allocator = Arena_Empty();

    A *a = Arena_New(&allocator, A);
    *a = (A) {.Int = 42, .Str = "hello world"};
    printf("%d %s\n", a->Int, a->Str);

    A *b = Arena_Copy(&allocator, &((A) {.Int=33, .Str="amogus"}), sizeof(A));
    printf("%d %s\n", b->Int, b->Str);

    int *nums = Arena_NewArray(&allocator, int, 100);
    for (size_t i = 0; i < 100; i++) {
        printf("\r%d\r", nums[i]);
    }

    int *nums1 = Arena_Allocate(&allocator, sizeof(int) * 250, alignof(int));
    for (size_t i = 0; i < 250; i++) {
        printf("\r%d\r", nums1[i]);
    }

    int *nums2 = Arena_Allocate(&allocator, sizeof(int) * 2000, alignof(int));
    for (size_t i = 0; i < 100; i++) {
        printf("\r%d\r", nums2[i]);
    }

    Arena_Free(&allocator);

    return EXIT_SUCCESS;
}
