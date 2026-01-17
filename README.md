# c-memory-tracker

A small C utility for tracking heap allocations during development.

This project wraps `malloc` and `free` to record where memory is allocated and to report allocations that were not freed when the program exits. It is intended as a learning and debugging tool, not a production memory manager.

---

## Description

`c-memory-tracker` helps make memory leaks visible by keeping a record of active heap allocations and printing a report at program termination. The goal of the project is to better understand memory ownership, allocation lifetimes, and basic debugging techniques in C.

The implementation is intentionally simple and focuses on correctness and readability.

---

## Features

- Wraps standard `malloc` and `free`
- Tracks active heap allocations using a singly linked list
- Records allocation metadata:
  - pointer address
  - allocation size
  - source file
  - source line
- Automatically reports unfreed allocations when the program exits normally

---

## Limitations and non-goals

This project does **not** attempt to:

- replace the system allocator
- manage or optimize heap memory
- provide thread safety
- support `realloc`
- automatically free leaked memory

The tracker only reports issues; it does not attempt to correct them.

---

## How it works

1. Including `memtrack.h` replaces calls to `malloc` and `free` using macros.
2. `mt_malloc` calls the real libc `malloc` and records metadata about the allocation.
3. `mt_free` removes the corresponding record and calls the real libc `free`.
4. `mt_report` is registered using `atexit` and runs automatically when the program exits.
5. Any remaining records are printed as memory leaks.

---

## Ownership model

- All allocated memory is owned by the caller.
- The tracker owns only its internal bookkeeping records.
- Memory is freed only when the caller explicitly calls `free`.
- Leaked memory is reported but not cleaned up.

This mirrors the behavior of common memory debugging tools.

---

## Public interface

```c
void *mt_malloc(size_t size, const char *file, int line);
void  mt_free(void *ptr);
void  mt_report(void);
```

Macro behavior
Including memtrack.h replaces malloc and free in the current source file:

#define malloc(x) mt_malloc((x), __FILE__, __LINE__)
#define free(x)   mt_free((x))
Macro replacement can be disabled by defining MT_DISABLE_MACROS before including the header, or by compiling with -DMT_DISABLE_MACROS.

Example
#include "memtrack.h"

int main(void)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));

    free(a);
    /* b is intentionally not freed */

    return 0;
}
Example output
========================================
[memtrack] MEMORY LEAK REPORT
----------------------------------------
Leak #1: ptr=0x55c9e3c1c2a0 size=4 allocated at main.c:6
----------------------------------------
Leaked blocks: 1
Leaked bytes : 4
========================================
Tests
A basic test is provided in tests/test_basic.c.

The test allocates three blocks, frees two of them, and intentionally leaks one to demonstrate the reporting behavior.

Purpose
This project was built as a learning exercise to improve understanding of heap allocation, ownership, and debugging in C. It is intended for educational and portfolio use.

