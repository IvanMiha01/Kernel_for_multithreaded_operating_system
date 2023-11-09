#include "../h/kSlab.hpp"

kSlab *kSlab::createSlab(size_t slotSize, int numOfSlots) {
    kSlab* slab=(kSlab*)kbuddyAllocator::alloc(sizeof(kSlab));
    if(slab==0) return nullptr;//error, failed allocation of new slab
    slab->next=nullptr;
    slab->slotSize=slotSize;
    slab->numOfSlots=numOfSlots;
    slab->numOfFreeSlots=numOfSlots;
    slab->freeSlots=(int*)kbuddyAllocator::alloc(numOfSlots*sizeof(int));
    if(slab->freeSlots==nullptr){
        kbuddyAllocator::free(slab, sizeof(kSlab));
        return nullptr;//error, failed allocation of freeSlots array
    }
    for(int i=0;i<numOfSlots;i++) (slab->freeSlots)[i]=1;
    slab->slots=(char*)kbuddyAllocator::alloc(numOfSlots*slotSize);
    if(slab->slots== nullptr){
        kbuddyAllocator::free(slab->freeSlots, numOfSlots*sizeof(int));
        kbuddyAllocator::free(slab, sizeof(kSlab));
        return nullptr;//error, failed allocation of slots array
    }
    return slab;
}

void *kSlab::alloc() {
    if(numOfFreeSlots==0) return nullptr;//slab is full
    int i=0;
    for(i=0; i<numOfSlots; i++){
        if(freeSlots[i]) break;
    }
    if(!freeSlots[i]) return nullptr;//should never happen
    freeSlots[i]=false;
    numOfFreeSlots--;
    return (void*)(slots+i*slotSize);
}

int kSlab::free(void *ptr) {
    if(!isHere(ptr)) return -1;//object is not in this slab
    int i=((char*)ptr-slots)/slotSize;
    if(freeSlots[i]) return 1;//already free
    freeSlots[i]=true;
    numOfFreeSlots++;
    return 0;
}

void kSlab::destroySlab() {
    kbuddyAllocator::free(slots,numOfSlots*slotSize);
    kbuddyAllocator::free(freeSlots, numOfSlots*sizeof(int));
    setNext(nullptr);
    //kbuddyAllocator::free(this,sizeof(kSlab));
    return;
}

bool kSlab::isHere(void *ptr) {
    if(ptr== nullptr)  return false;
    return ((char*)ptr>=slots && (char*)ptr<=slots+numOfSlots*slotSize);
}


