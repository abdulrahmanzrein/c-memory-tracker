/*
    memtrack.c

    A small debugging tool for tracking heap allocations in C.

    This file implements the logic behind memtrack.h.
    The idea is simple: every allocation is written down, every free
    crosses it off the list, and whatever is left at the end is a leak.

    This is NOT a custom allocator.
    It does not manage memory or try to fix leaks.
    It only records what happens and reports it.
*/


#define MT_DISABLE_MACROS  // Disable malloc/free macro rewriting in THIS file only
#include "memtrack.h"

#include <stdio.h>   // fprintf
#include <stdlib.h>  // malloc, free, atexit
#include <stddef.h>  // size_t


typedef struct mt_record {
    void *ptr;                // pointer returned to the user
    size_t size;              // size requested by the user
    const char *file;         // __FILE__ where allocation happened
    int line;                 // __LINE__ where allocation happened
    struct mt_record *next;   // next record in the list
} mt_record_t;


// Head of the singly linked list of "live allocations".
static mt_record_t *g_head = NULL;

// Simple counters to make reports easier to read
static size_t g_live_blocks = 0;
static size_t g_live_bytes  = 0;

// Ensures we only register mt_report() once
static int g_report_registered = 0;


void *mt_malloc(size_t size, const char *file, int line) {
    /*
        Register the leak report the first time we see an allocation.
        atexit() will call mt_report() when the program exits normally.
    */
    if (!g_report_registered) {
        atexit(mt_report);
        g_report_registered = 1;
    }

    // Allocate user memory
    void *ptr = malloc(size);
    if(ptr == NULL) {
        return NULL; // malloc failed
    }

    // Create a new record
    mt_record_t *record = (mt_record_t *)malloc(sizeof(mt_record_t));
    if(record == NULL) {
        free(ptr); // Clean up user memory on failure
        return NULL;
    }

    record->ptr  = ptr;
    record->size = size;
    record->file = file;
    record->line = line;

    // Insert the record at the head of the list
    record->next = g_head;
    g_head = record;

    // Update counters
    g_live_blocks++;
    g_live_bytes += size;

    return ptr;
}


void mt_free(void *ptr) {
    if(ptr == NULL) {
        free(NULL);
        return;
    }

    mt_record_t *current = g_head;
    mt_record_t *previous = NULL;

    // Search for the record in the list
    while(current != NULL) {
        if(current->ptr == ptr) {
            // Found the record, remove it from the list
            if(previous == NULL) {
                g_head = current->next; // Removing head
            } else {
                previous->next = current->next;
            }

            // Update counters
            g_live_blocks--;
            g_live_bytes -= current->size;

            // Free the record and user memory
            free(current);
            free(ptr);
            return;
        }
        previous = current;
        current = current->next;
}

    // If we reach here, the pointer was not found
    fprintf(stderr,
            "[memtrack] Warning: free(%p) not found in tracker records\n",
            ptr);
    free(ptr);
}

void mt_report(void)
{
    if (g_head == NULL) {
        fprintf(stderr, "[memtrack] No leaks detected.\n");
        return;
    }

    fprintf(stderr, "========================================\n");
    fprintf(stderr, "[memtrack] MEMORY LEAK REPORT\n");
    fprintf(stderr, "----------------------------------------\n");

    size_t leaks = 0;
    size_t bytes = 0;

    for (mt_record_t *cur = g_head; cur != NULL; cur = cur->next) {
        leaks++;
        bytes += cur->size;

        fprintf(stderr,
                "Leak #%zu: ptr=%p size=%zu allocated at %s:%d\n",
                leaks, cur->ptr, cur->size, cur->file, cur->line);
    }

    fprintf(stderr, "----------------------------------------\n");
    fprintf(stderr, "Leaked blocks: %zu\n", leaks);
    fprintf(stderr, "Leaked bytes : %zu\n", bytes);
    fprintf(stderr, "========================================\n");
}