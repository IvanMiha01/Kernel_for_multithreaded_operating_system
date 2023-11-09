#ifndef PROJECT_BASE_SLABALLOCATOR_HPP
#define PROJECT_BASE_SLABALLOCATOR_HPP

#include "../h/kCache.hpp"

class slabAllocator {

public:
    static kCache* kTCBCache;
    static kCache* kSemaphoreCache;
    static kCache* kListElemCache;
    static kCache* ktimeSleepQueueElemCache;
    static kCache* stackCache;
    static void init();
    static void* kalloc(size_t size);
    static int kfree(void* ptr);
    static void print();
    static void destroyCache(kCache* cache);
    static kCache* createCache(const char* name, size_t objSize, void (*ctor)(void * ctorArgs), void (*dtor)(void *));
private:
    static kCache* head;
};


#endif //PROJECT_BASE_SLABALLOCATOR_HPP
