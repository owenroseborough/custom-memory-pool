#include <iostream>
#include <vector>
#include <cassert>
#include <memory>

#include "get-cache-line-size.h"
using namespace std;

template <typename T>
class MemoryPool {
private:
    size_t chunkSize_;         // The size of each memory chunk
    size_t chunkAlign_;        // The alignment of memory chunks
    size_t poolSize_;          // Number of chunks in the pool
    void* pool_;               // The large block of memory
    std::vector<T*> freeList_; // Free list to track available memory chunks
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

            if (cacheLinesPerObject < 1) {
                size_t poolSize = (ceil(static_cast<double>(poolSize_) / numObjectsPerCacheLine) * cacheLineSize) + chunkAlign_;
            }
            else {
                size_t poolSize = poolSize_ * ceil(cacheLinesPerObject) * cacheLineSize + chunkAlign_;
            }
            // Allocate a large block of memory for the pool
            pool_ = malloc(poolSize);
            assert(pool_ != nullptr); // Ensure memory was allocated

            // Align pool_ pointer to chunkAlign_ requirement
            // use std::align here to get first aligned address of T from pool_
            // need pointer address to be aligned according to chunkAlign_
            pool_ = align(chunkAlign_, chunkSize_, pool_, poolSize);

            // TODO: this below function wrong for types longer than cache line likely
            // Initialize the free list with all memory chunks

            // what we want:
            // we are pushing addresses onto the freeList
            
            // start address
            // end address

            // calculate how many cache lines one object needs

            // if larger than 1, next allocation will be at starting pool address 
            // plus 2

            // if larger than 2, next allocation will be at starting pool address
            // plus 3

            // 00
            // 64
            // 128
            // 192
            if (cacheLinesPerObject < 1) {
                for (size_t i = 0; i < ceil(poolSize_ / numObjectsPerCacheLine);) {
                    for (size_t j = 0; j < numObjectsPerCacheLine; j++) {
                        freeList_.push_back(static_cast<T*>(pool_) + (i * cacheLineSize) + (j * blockCounter * chunkAlign_));
                    }
                }
            }
            else {
                // TODO: handle big objects
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
