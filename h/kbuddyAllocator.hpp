#ifndef PROJECT_BASE_KBUDDYALLOCATOR_HPP
#define PROJECT_BASE_KBUDDYALLOCATOR_HPP

#include "../lib/hw.h"
#include "../h/kprint.hpp"

class kbuddyAllocator {

public:
    static void init();

    static void* alloc(size_t size);

    static void free(void* addr, size_t size);

    static void print();
private:
    static const int maxPow = 24;
    static int freeChunks[maxPow + 1];
};


#endif //PROJECT_BASE_KBUDDYALLOCATOR_HPP
