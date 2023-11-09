#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct FreeMem{
    FreeMem* next;
    size_t size;
};

class kmemoryAllocator {

public:

    static void init();

    static void* mem_alloc(size_t size);

    static int mem_free(void* address);

    static size_t checkSize();

private:

    static FreeMem* head;
};


#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP
