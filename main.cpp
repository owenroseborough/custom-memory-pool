
#include "custom-memory-pool.h"


int main() {
    // Create a memory pool for MyClass with chunks of size equal to the size of MyClass
    MemoryPool<int> pool(static_cast<size_t>(10), "CacheLineAlignment"); // Pool with 10 ints

    // Allocate an object from the memory pool
    //MyClass* obj1 = new(pool.allocate()) MyClass(10); // Placement new
    //std::cout << "Object 1 x: " << obj1->x << std::endl;

    // Allocate another object
    //MyClass* obj2 = new(pool.allocate()) MyClass(20); // Placement new
    //std::cout << "Object 2 x: " << obj2->x << std::endl;

    // Deallocate objects
    //pool.deallocate(obj1);
    //pool.deallocate(obj2);

    return 0;
}
