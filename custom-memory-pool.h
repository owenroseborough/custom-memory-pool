#include <iostream>
#include <vector>
#include <cassert>
#include <memory>
#include "get-cache-line-size.h"
using namespace std;

template <typename T>
class MemoryPool {
private:
    size_t chunkSize_;            // The size of each memory chunk
    size_t chunkAlign_;           // The alignment of memory chunks
    size_t poolSize_;             // Number of chunks in the pool
    size_t cacheLinesPerObject_;  // Number of cache lines per object
    size_t numItemsPerCacheLine_; // Number of items per cache line
    size_t poolByteSize_;         // Number of bytes in pool
    void* pool_;                  // The large block of memory
    std::vector<T*> freeList_;    // Free list to track available memory chunks
public:
    // Constructor to initialize the memory pool of template type
    MemoryPool(size_t poolSize, string alignment = "CacheLineAlignment"){
        
        chunkSize_ = sizeof(T);
        chunkAlign_ = alignof(T);
        poolSize_ = poolSize;

        // for write-heavy workloads that require cache alignment
        if (alignment == "CacheLineAlignment") {
            size_t cacheLineSize = getCacheLineSize();   // typically 64 byte
            size_t blockCounter = ceil(static_cast <double>(chunkSize_) / chunkAlign_);

            numItemsPerCacheLine_ = floor(cacheLineSize / (blockCounter * chunkAlign_));
            double cacheLinesPerObject = (static_cast<double>(blockCounter) * chunkAlign_) / cacheLineSize;

            if (cacheLinesPerObject < 1) {
                cacheLinesPerObject_ = 0;
                poolByteSize_ = ceil(static_cast<double>(poolSize_) / numItemsPerCacheLine_) * cacheLineSize;
            }
            else {
                numItemsPerCacheLine_ = 1;
                cacheLinesPerObject_ = ceil(cacheLinesPerObject);
                poolByteSize_ = poolSize_ * ceil(cacheLinesPerObject) * cacheLineSize;
            }
            // Allocate a large block of memory for the pool
            pool_ = malloc(poolByteSize_);
            if (pool_ == nullptr) {  // Ensure memory was allocated
                cerr << "Memory allocation failed!" << endl;
                terminate();
            }

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
    size_t getCacheLinesPerObject() {
        return cacheLinesPerObject_;
    }
    size_t getLengthFreeList() {
        return freeList_.size();
    }
    size_t getNumItemsPerCacheLine() {
        return numItemsPerCacheLine_;
    }
    size_t getPoolByteSize() {
        return poolByteSize_;
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
