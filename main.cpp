#include <iostream>
#include <cstring>
#include "memory_region.hpp"

int main() {

    gmem Allocator;

    std::cout << "Heap Allocation Tests: \n";

    char *p1 = static_cast<char*>(Allocator.heap_alloc(1000));
    char *p2 = static_cast<char*>(Allocator.heap_alloc(1000));
    char *p3 = static_cast<char*>(Allocator.heap_alloc(1000));

    if(!p1 || !p2 || !p3) {
        std::cerr << "Initial allocation failed\n";
        return 1;
    }

    std::cout << "Allocation: \n";

    std::cout << "p1: " << static_cast<void*>(p1) << " (1000 bytes)\n";
    std::cout << "p2: " << static_cast<void*>(p2) << " (1000 bytes)\n";
    std::cout << "p3: " << static_cast<void*>(p3) << " (1000 bytes)\n";


    // Write data into allocated memory
    std::strcpy(p1, "Hello from allocation 1!");
    std::strcpy(p2, "Hello from allocation 2!");
    std::strcpy(p3, "Hello from allocation 3!");

    std::cout << "\nReading Data: \n";

    std::cout << "p1: " << p1 << '\n';
    std::cout << "p2: " << p2 << '\n';
    std::cout << "p3: " << p3 << '\n';


    // Free p1 and p3
    std::cout << "\nFreeing Memory: \n";

    Allocator.heap_free(p1);
    Allocator.heap_free(p3);

    std::cout << "Freed p1 and p3\n";


    // Try allocating another block
    std::cout << "\nReusing Freed memory: \n";

    char *p4 = static_cast<char*>(Allocator.heap_alloc(1000));

    if(p4) {
        std::cout << "p4: " << static_cast<void*>(p4) << '\n';

        std::strcpy(p4, "This memory has been reused!");

        std::cout << "p4 contents: " << p4 << '\n';
    }
    else {
        std::cout << "Allocation failed\n";
    }


    // Free all currently allocated blocks
    Allocator.heap_free(p2);

    if(p4)
        Allocator.heap_free(p4);

    std::cout << "\nFragmentation: \n";

    std::cout << "All allocated blocks have now been freed.\n";
    std::cout << "However, adjacent free chunks have NOT been merged.\n";

    // Request a large contiguous block
    char *p5 = static_cast<char*>(Allocator.heap_alloc(2500));

    if(p5) {
        std::cout << "p5 allocated successfully at: "
                  << static_cast<void*>(p5) << '\n';
    }
    else {
        std::cout << "p5 allocation FAILED (2500 bytes)\n";
        std::cout << "There may be enough total free memory, "
                     "but no individual free chunk is large enough.\n";
    }

    return 0;
}
