# MemoryPool

A flexible memory pool implementation in C++ for efficient memory allocation and deallocation of user-defined types.

## Features
- alignment options for cache-friendly design.
- Efficient allocation and deallocation using a free list.
- Support for write-heavy workloads requiring cache-line alignment.
- Alignment-independent allocation for read-heavy workloads.
- Safety features to prevent invalid operations.

## Requirements
- C++17 or later
- A platform supporting aligned memory allocation
- `get-cache-line-size.h` header (to provide cache line size functionality)

### Key Properties
- **Chunk Size**: The size of each memory block.
- **Chunk Alignment**: Alignment for each memory block.
- **Pool Size**: Total number of chunks in the memory pool.
- **Cache Lines Per Object**: Cache lines occupied by a single object.
- **Free List**: Tracks available memory blocks for allocation.
- **Taken Map**: Tracks allocated memory blocks.

## Usage

### 1. Initialization
The `MemoryPool` must be initialized with a template type and a pool size. Optionally, cache-line alignment can be enabled for write-heavy workloads.

```cpp
#include "MemoryPool.h"

MemoryPool<int> pool(1000, "CacheLineAlignment"); // Memory pool for 1000 integers
```

### 2. Memory Allocation
Allocate memory chunks from the pool using the `allocate` method:

```cpp
int* ptr = pool.allocate();
if (ptr != nullptr) {
    *ptr = 42; // Use the allocated memory
}
```

### 3. Memory Deallocation
Deallocate memory chunks using the `deallocate` method:

```cpp
pool.deallocate(ptr);
```

### 4. Query Pool Properties
Access pool properties for debugging or performance analysis:

```cpp
size_t freeListSize = pool.getLengthFreeList();
size_t poolSizeBytes = pool.getPoolByteSize();
```

## Constructor Parameters
```cpp
MemoryPool(size_t poolSize, std::string alignment = "CacheLineAlignment");
```
- **poolSize**: Total number of chunks to allocate.
- **alignment**: Type of alignment (`CacheLineAlignment` or default alignment).

### Constructor Behavior
- For `CacheLineAlignment`: Ensures memory chunks are aligned to cache lines for performance in write-heavy workloads.
- For default alignment: Allocates memory without specific alignment, useful for read-heavy workloads.

## Error Handling
- Throws `std::invalid_argument` if `poolSize` is zero.
- Logs errors to `stderr` if memory allocation fails.
- Prevents deallocating memory not allocated by the pool.

## Example
```cpp
#include <iostream>
#include "MemoryPool.h"

int main() {
    MemoryPool<int> pool(1000, "CacheLineAlignment");

    // Allocate memory
    int* a = pool.allocate();
    int* b = pool.allocate();

    // Use the allocated memory
    if (a) *a = 10;
    if (b) *b = 20;

    std::cout << "a: " << *a << ", b: " << *b << std::endl;

    // Deallocate memory
    pool.deallocate(a);
    pool.deallocate(b);

    return 0;
}
```

## Advantages
- **Performance**: Reduced allocation overhead and optimized for cache efficiency.
- **Safety**: Strong type-checking and prevention of double-free or invalid memory operations.
- **Flexibility**: Adaptable to different workloads and memory alignment requirements.

## Limitations
- The pool size must be determined during initialization and cannot be dynamically resized.
- Memory for objects with non-trivial destructors must be explicitly managed.

## License
This project is available under the MIT License.

---

For questions or contributions, feel free to reach out!

