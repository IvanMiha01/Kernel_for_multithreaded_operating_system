#ifndef PROJECT_BASE_KCACHE_HPP
#define PROJECT_BASE_KCACHE_HPP

#include "kSlab.hpp"

class kCache {

public:
    static kCache* createCache(const char* name, size_t objSize, void (*ctor)(void * ctorArgs), void (*dtor)(void *));
    void* alloc();
    int free(void* ptr);
    void static destroyCache(kCache* c);
    size_t getObjSize(){return objSize; }
    bool isHere(void* ptr);
    kCache* getNext(){return next; }
    void setNext(kCache* c){ next=c; }
    void print();

    int shrink();
private:
    char name[64];
    size_t objSize;
    kCache* next;

    void (*ctor)(void *);
    void (*dtor)(void *);
    kSlab* fullSlabs;
    kSlab* halfFullSlabs;
    kSlab* emptySlabs;
    static size_t findOptimalSlabSize(size_t objSize);

};
#endif //PROJECT_BASE_KCACHE_HPP
