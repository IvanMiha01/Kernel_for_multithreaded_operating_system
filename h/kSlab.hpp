#ifndef PROJECT_BASE_KSLAB_HPP
#define PROJECT_BASE_KSLAB_HPP


#include "../lib/hw.h"
#include "../h/kbuddyAllocator.hpp"

class kSlab {

public:
    static kSlab* createSlab(size_t slotSize, int numOfSlots);
    void destroySlab();
    void* alloc();
    int free(void* ptr);
    bool isFull() { return numOfFreeSlots==0;}
    bool isEmpty() { return numOfFreeSlots == numOfSlots; }
    bool isHere(void* ptr);
    int getNumOfFreeSlots(){return numOfFreeSlots; }
    kSlab* getNext(){ return next; }
    void setNext(kSlab* s){ next = s; }
private:
    kSlab* next;
    int slotSize;
    int numOfSlots;
    int numOfFreeSlots;
    int* freeSlots;
    char* slots;
};


#endif //PROJECT_BASE_KSLAB_HPP
