#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <memory>
#include <type_traits>
#include "get-cache-line-size.h"
using namespace std;

template <typename T>
class MemoryPool {
    static_assert(!is_same_v<T, void>, "MemoryPool cannot be instantiated with void");
    static_assert(!is_same_v<T, void*>, "MemoryPool cannot be instantiated with void*");
private:
    size_t chunkSize_;            // The size of each memory chunk
    size_t chunkAlign_;           // The alignment of memory chunks
    size_t poolSize_;             // Number of chunks in the pool
    size_t cacheLinesPerObject_;  // Number of cache lines per object
    size_t numItemsPerCacheLine_; // Number of items per cache line
    size_t poolByteSize_;         // Number of bytes in pool
    void* pool_;                  // The large block of memory
    vector<T*> freeList_;         // Free list to track available memory chunks
    map<T*, bool> takenMap_;      // taken map to track allocated memory chunks
public:
    MemoryPool() = delete;                             // delete default constructor
    MemoryPool(const MemoryPool&) = delete;            // delete copy constructor
    MemoryPool(MemoryPool&&) = delete;                 // delete move constructor
    MemoryPool& operator=(const MemoryPool&) = delete; // delete copy assignment operator
    MemoryPool& operator=(MemoryPool&&) = delete;      // delete move assignment operator

    // Constructor to initialize the memory pool of template type
    MemoryPool(size_t poolSize, string alignment = "CacheLineAlignment"){
        
        (poolSize == (size_t)0) ? throw invalid_argument::invalid_argument("MemoryPool cannot be instantiated with poolSize 0"): NULL ;

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
            // Allocate a large block of memory for the pool
            poolByteSize_ = poolSize_ * chunkSize_;
            pool_ = malloc(poolSize_ * chunkSize_);
            if (pool_ == nullptr) {  // Ensure memory was allocated
                cerr << "Memory allocation failed!" << endl;
                terminate();
            }
            // since in this type of allocation we are not cache line aligned
            numItemsPerCacheLine_ = 0;  
            cacheLinesPerObject_ = 0;

            size_t objectCounter = 0;
            char* startAllocAddress = static_cast<char*>(pool_);
            while (objectCounter < poolSize_) {
                freeList_.push_back(static_cast<T*>(static_cast<void*>(startAllocAddress)));
                startAllocAddress += chunkSize_;
                objectCounter++;
            }
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
    shared_ptr<T*> allocate() {
        if (freeList_.empty()) {
            std::cerr << "Memory pool out of memory!" << std::endl;
            return nullptr; // Handle allocation failure
        }
        // Pop a chunk from the free list
        T* chunk = freeList_.back();
        takenMap_[chunk] = true;
        freeList_.pop_back();
        return make_shared<T*>(chunk);
    }
    // Deallocate a chunk of memory
    void deallocate(T* ptr) {
        if (takenMap_.contains(ptr)) {
            takenMap_.erase(ptr);
            freeList_.push_back(ptr); // Push the chunk back onto the free list
        }
        else {
            cerr << "ptr to deallocate not found in memory pool!" << endl;
        }
    }
    // defragment the memory pool to increase memory efficiency
    // coalescing adjacent free blocks to reduce fragmentation
    void coalesceMemoryPool() {
        // if we are to coalesce memory pool, then no other thread can be accessing the memory
        // locations
        // if a thread is reading from a location and we try to move it, that would be wrong
        // if a thread is modifying a location and we try to move it that would be wrong
        // if we return shared pointers to memory addresses, how to guarantee that no one is modifying
        // the locations?
    }
};
