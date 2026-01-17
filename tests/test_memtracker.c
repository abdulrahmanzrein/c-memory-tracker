#include <stdio.h>
#include <stdlib.h>

#include "../src/memtrack.h"

int main(void)
{
    printf("Starting basic memory tracker test...\n");

    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));

    if (!a || !b || !c) {
        printf("Allocation failed\n");
        return 1;
    }

    *a = 10;
    *b = 20;
    *c = 30;

    printf("Allocated three integers\n");

    // Free two of them
    free(a);
    free(b);

    printf("Freed two integers\n");

    // Intentionally leak `c`
    printf("Leaving one allocation unfreed on purpose\n");

    printf("Exiting program...\n");
    return 0;
}