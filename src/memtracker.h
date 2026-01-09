#ifndef MEMTRACK_H
#define MEMTRACK_H

/*
    memtrack.h

    A small debugging tool for tracking heap allocations in C.

    This is NOT a custom allocator.
    It does not manage memory, optimize anything, or replace libc.

    All it does is keep a list of "what has been malloc'd but not freed"
    so leaks are easy to spot during development.
*/

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

/*
    mt_malloc

    Allocates memory using the normal libc malloc, then records
    where the allocation came from (file + line).

    The returned memory belongs entirely to the caller.
    The tracker only keeps metadata about it.
*/
void *mt_malloc(size_t size, const char *file, int line);

/*
    mt_free

    Frees memory using the normal libc free and removes the
    corresponding allocation record from the tracker.

    The tracker never frees user memory on its own.
    If mt_free is not called, the memory is considered leaked.
*/
void mt_free(void *ptr);

/*
    mt_report

    Prints all allocations that are still live.

    Anything reported here represents memory that was allocated
    but never freed.

    This function only reports leaks — it does not clean them up.
*/
void mt_report(void);

#ifdef __cplusplus
}
#endif

/*
    Macro layer

    Including this header normally replaces malloc/free in the
    current source file with mt_malloc/mt_free.

    This lets us capture file and line information automatically
    without changing existing code.

    If macro replacement causes issues (or is not wanted),
    it can be disabled by defining MT_DISABLE_MACROS before
    including this header or by compiling with -DMT_DISABLE_MACROS.
*/
#ifndef MT_DISABLE_MACROS
    #define malloc(x) mt_malloc((x), __FILE__, __LINE__)
    #define free(x)   mt_free((x))
#endif

#endif // MEMTRACK_H