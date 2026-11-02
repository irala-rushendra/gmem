# gmem - Custom Heap Allocator

A small heap allocator built from scratch in **C++17**, using Linux
`mmap()` to acquire memory and a first-fit strategy to manage
allocations.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey?logo=linux)
![Status](https://img.shields.io/badge/status-V1-informational)

------------------------------------------------------------------------

## Overview

`gmem` is an educational heap allocator that manages a memory region
obtained directly from the operating system with `mmap()`.

It maintains metadata for each memory chunk, searches for free chunks
using **First Fit**, splits chunks when allocating, and marks chunks as
free when they are released.

The project is intended to explore how heap allocators work internally,
including block metadata, allocation strategies, memory reuse, and
external fragmentation.

## How it works

The allocator requests one page of memory from the operating system. The
beginning of that region stores the first chunk's metadata; the
remaining space is available for allocations.

``` mermaid
flowchart TD
    A["gmem constructor"] --> B["heap_init()"]
    B --> C["mmap() obtains a memory region"]
    C --> D["Placement-new constructs first chunk header"]
    D --> E["heap_alloc(size)"]
    E --> F["First-fit search"]
    F --> G{"Free chunk large enough?"}
    G -- No --> H["Return nullptr"]
    G -- Yes --> I{"Enough room to split?"}
    I -- Yes --> J["Split chunk and create new header"]
    I -- No --> K["Use the whole chunk"]
    J --> L["Return payload address"]
    K --> L
    L --> M["heap_free(ptr) marks chunk free"]
    M --> N["Destructor calls munmap()"]
```

### Chunk layout

Each chunk has a metadata header followed by its payload:

``` text
+----------------------+---------------------------+
|     heapchunk_t      |       payload             |
+----------------------+---------------------------+
| next | size | in_use |                           |
+----------------------+---------------------------+
```

The linked list tracks both allocated and free chunks. Allocation walks
this list from the beginning and selects the first suitable free chunk.

## Features in V1

-   Acquire a memory region using Linux `mmap()`.
-   Construct chunk metadata using placement `new`.
-   First-fit allocation.
-   Split a free chunk when enough space remains for another header and
    payload.
-   Free a chunk by marking it available for reuse.
-   Track aggregate available payload space.
-   Release the mapped region using `munmap()`.

## Demonstration

The test program exercises allocation, writing and reading data, freeing
blocks, reusing a freed block, and observing fragmentation.

Example output (addresses will differ between runs):

``` text
Heap Allocation Tests:
Allocation:
p1: 0x4037018 (1000 bytes)
p2: 0x4037418 (1000 bytes)
p3: 0x4037818 (1000 bytes)

Reading Data:
p1: Hello from allocation 1!
p2: Hello from allocation 2!
p3: Hello from allocation 3!

Freeing Memory:
Freed p1 and p3

Reusing Freed memory:
p4: 0x4037018
p4 contents: This memory has been reused!

Fragmentation:
All allocated blocks have now been freed.
However, adjacent free chunks have NOT been merged.
p5 allocation FAILED (2500 bytes)
There may be enough total free memory, but no individual free chunk is large enough.
```

The repeated address for `p4` demonstrates reuse of the freed `p1`
chunk.

## External fragmentation

V1 does not coalesce adjacent free chunks. As a result, free space can
become divided into multiple smaller chunks.

``` mermaid
flowchart TB
    subgraph Before["Free chunks after deallocation"]
        A["Free block<br/>~1000 B"]
        B["Free block<br/>~1000 B"]
        C["Free block<br/>~1000 B"]
        D["Free block<br/>remaining space"]
    end
    E["A large request may fail<br/>if no single free chunk is large enough"]
    A --> E
    B --> E
    C --> E
    D --> E
```

Even when the sum of free payload space is sufficient, an allocation can
fail if no individual free chunk can satisfy the request. This is
**external fragmentation**.

Coalescing adjacent free chunks is a planned improvement.

## Build and run

### Requirements

-   Linux
-   A C++17-compatible compiler (such as `g++`)
-   GNU Make
-   Valgrind (optional, for diagnostics)

### Compile

``` bash
make allocator
```

Or compile directly:

``` bash
g++ -std=c++17 -Wall -Wextra -g memory_region.cpp main.cpp -o allocator
```

### Run

``` bash
./allocator
```

### Clean

``` bash
make clean
```

## Valgrind

Run the test executable with:

``` bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./allocator
```

A test run during V1 development reported:

``` text
in use at exit: 0 bytes in 0 blocks
ERROR SUMMARY: 0 errors from 0 contexts
```

This is a useful diagnostic result for that run, not a guarantee that
every allocator path is free of bugs. Valgrind's standard heap summary
also does not fully describe the allocator's internal chunk bookkeeping.

## Current limitations

V1 is deliberately small and has several limitations:

-   No coalescing of adjacent free chunks.
-   Alignment handling is limited; allocation sizes must respect the
    alignment assumptions of the implementation.
-   Uses a single mapped memory region.
-   No support for growing the heap when the region is exhausted.
-   Invalid pointers passed to `heap_free()` are not reported to the
    caller.
-   Not intended as a production general-purpose allocator.

## Planned improvements

-   [ ] Coalesce adjacent free chunks.
-   [ ] Implement robust alignment handling.
-   [ ] Add more allocation and fragmentation tests.
-   [ ] Handle heap exhaustion and mapping failures more
    comprehensively.
-   [ ] Add allocator statistics and diagnostics.

## Project structure

``` text
gmem/
├── memory_region.hpp
├── memory_region.cpp
├── main.cpp
├── Makefile
└── README.md
```

## License

Add a license if you intend to distribute or reuse this project
publicly.
