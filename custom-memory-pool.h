#include <iostream>
#include <vector>
#include <cassert>
#include <memory>

#include "get-cache-line-size.h"
using namespace std;

template <typename T>
class MemoryPool {
private:
    size_t chunkSize_;           // The size of each memory chunk
    size_t chunkAlign_;          // The alignment of memory chunks
    size_t poolSize_;            // Number of chunks in the pool
    size_t cacheLinesPerObject_; // Number of cache lines per object
    void* pool_;                 // The large block of memory
    std::vector<T*> freeList_;   // Free list to track available memory chunks
public:
    // Constructor to initialize the memory pool of template type
    MemoryPool(size_t poolSize, string alignment = "CacheLineAlignment"){
        
        chunkSize_ = sizeof(T);
        chunkAlign_ = alignof(T);
        poolSize_ = poolSize;

        // for write-heavy workloads that require cache alignment
        if (alignment == "CacheLineAlignment") {
            size_t cacheLineSize = getCacheLineSize();   // typically 64 byte
            size_t blockCounter = 0;
            size_t remainder = chunkSize_;
            while (remainder > chunkAlign_) {
                remainder -= chunkAlign_;
                blockCounter++;
            }
            blockCounter++;

            double cacheLinesPerObject = (static_cast<double>(blockCounter) * chunkAlign_) / cacheLineSize;
            size_t numObjectsPerCacheLine = (cacheLineSize / chunkAlign_) / blockCounter;

            size_t poolSize;
            if (cacheLinesPerObject < 1) {
                cacheLinesPerObject_ = 0;
                poolSize = (ceil(static_cast<double>(poolSize_) / numObjectsPerCacheLine) * cacheLineSize) + chunkAlign_;
            }
            else {
                cacheLinesPerObject_ = ceil(cacheLinesPerObject);
                poolSize = poolSize_ * ceil(cacheLinesPerObject) * cacheLineSize + chunkAlign_;
            }
            // Allocate a large block of memory for the pool
            pool_ = malloc(poolSize);
            assert(pool_ != nullptr); // Ensure memory was allocated

            // Align pool_ pointer to chunkAlign_ requirement
            pool_ = align(chunkAlign_, chunkSize_, pool_, poolSize);

            // TODO: test below section with different inputs
            size_t objectCounter = 0;
            char* startCacheLine = static_cast<char*>(pool_);
            char* endCacheLine = startCacheLine + cacheLineSize;
            char* startAllocAddress = static_cast<char*>(pool_);
            char* endAllocAddress;
            while (objectCounter < poolSize_) {
                endAllocAddress = startAllocAddress + (blockCounter * chunkAlign_);

                if (startAllocAddress != startCacheLine && endAllocAddress > endCacheLine) {
                    freeList_.push_back(static_cast<T*>(static_cast<void*>(startCacheLine + cacheLineSize)));
                    if (cacheLinesPerObject_ < 1) {  // object can fit on cache line
                        startAllocAddress = startCacheLine + cacheLineSize + (blockCounter * chunkAlign_);
                        startCacheLine = startCacheLine + cacheLineSize;
                    }
                    else {   // object larger than cache line
                        startAllocAddress = startCacheLine + cacheLineSize + (cacheLinesPerObject_ * cacheLineSize);
                        startCacheLine = startAllocAddress;
                    }
                    endCacheLine = startCacheLine + cacheLineSize;
                }
                else if (startAllocAddress == startCacheLine && endAllocAddress > endCacheLine) {
                    freeList_.push_back(static_cast<T*>(static_cast<void*>(startAllocAddress)));
                    startAllocAddress = startAllocAddress + cacheLinesPerObject_ * cacheLineSize;
                    startCacheLine = startAllocAddress;
                    endCacheLine = startCacheLine + cacheLineSize;
                }
                else {
                    freeList_.push_back(static_cast<T*>(static_cast<void*>(startAllocAddress)));
                    startAllocAddress += (blockCounter * chunkAlign_);
                }
                objectCounter++;
            }
        }
        // otherwise for read-heavy workloads that don't require cache line alignment
        else {
            cout << "placeholder";
        }
    }

    // Destructor to free the memory when done
    ~MemoryPool() {
        free(pool_);
    }

    // Allocate a chunk of memory
    void* allocate() {
        if (freeList_.empty()) {
            std::cerr << "Memory pool out of memory!" << std::endl;
            return nullptr; // Handle allocation failure
        }

        // Pop a chunk from the free list
        void* chunk = freeList_.back();
        freeList_.pop_back();
        return chunk;
    }

    // Deallocate a chunk of memory
    void deallocate(void* ptr) {
        freeList_.push_back(ptr); // Push the chunk back onto the free list
    }
};
