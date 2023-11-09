#include "../h/slabAllocator.hpp"
#include "../h/ktcb.hpp"
#include "../h/ksemaphore.hpp"

kCache* slabAllocator::kTCBCache= nullptr;
kCache* slabAllocator::kSemaphoreCache= nullptr;
kCache* slabAllocator::kListElemCache= nullptr;
kCache* slabAllocator::ktimeSleepQueueElemCache= nullptr;
kCache* slabAllocator::stackCache= nullptr;
kCache* slabAllocator::head= nullptr;

void slabAllocator::init() {
    kTCBCache = kCache::createCache("kTCBCache",sizeof(kTCB), nullptr, nullptr);
    kSemaphoreCache = kCache::createCache("kSemaphoreCache",sizeof(kSemaphore), nullptr, nullptr);
    kListElemCache = kCache::createCache("kListElemCache",2*sizeof(char*), nullptr, nullptr);
    ktimeSleepQueueElemCache = kCache::createCache("ktimeSleepQueueElemCache",2*sizeof(char*)+sizeof(uint64), nullptr, nullptr);
    stackCache = kCache::createCache("stackCache",DEFAULT_STACK_SIZE*4, nullptr, nullptr);
}

void *slabAllocator::kalloc(size_t size) {
    if(size>1<<17) return nullptr;//error, size of "small buffer" is too big
    if(size<1<<5) size=1<<5;
    size_t tmp=1<<5;
    while(tmp<size){
        tmp*=2;
    }
    size=tmp;
    for(kCache* c = head; c!= nullptr; c=c->getNext()){
        if(c->getObjSize()==size){
            return c->alloc();
        }
    }
    kCache* c = nullptr;
    if(size==1<<5){
        c=kCache::createCache("size-2^5",size, nullptr, nullptr);
    }
    else if(size==1<<6){
        c=kCache::createCache("size-2^6",size, nullptr, nullptr);
    }
    else if(size==1<<7){
        c=kCache::createCache("size-2^7",size, nullptr, nullptr);
    }
    else if(size==1<<8){
        c=kCache::createCache("size-2^8",size, nullptr, nullptr);
    }
    else if(size==1<<9){
        c=kCache::createCache("size-2^9",size, nullptr, nullptr);
    }
    else if(size==1<<10){
        c=kCache::createCache("size-2^10",size, nullptr, nullptr);
    }
    else if(size==1<<11){
        c=kCache::createCache("size-2^11",size, nullptr, nullptr);
    }
    else if(size==1<<12){
        c=kCache::createCache("size-2^12",size, nullptr, nullptr);
    }
    else if(size==1<<13){
        c=kCache::createCache("size-2^13",size, nullptr, nullptr);
    }
    else if(size==1<<14){
        c=kCache::createCache("size-2^14",size, nullptr, nullptr);
    }
    else if(size==1<<15){
        c=kCache::createCache("size-2^15",size, nullptr, nullptr);
    }
    else if(size==1<<16){
        c=kCache::createCache("size-2^16",size, nullptr, nullptr);
    }
    else if(size==1<<17){
        c=kCache::createCache("size-2^17",size, nullptr, nullptr);
    }
    if(c){
        c->setNext(head);
        head=c;
        return c->alloc();
    }
    return nullptr;//error, failed allocation of new cache
}

int slabAllocator::kfree(void *ptr) {
    if(ptr==nullptr) return -1;
    for(kCache* c=head;c!= nullptr;c=c->getNext()){
        if(c->isHere(ptr)){
            c->free(ptr);
            return 0;
        }
    }
    return -1;//error, object is not in caches
}

void slabAllocator::print() {
    for(kCache* c=head;c!= nullptr;c=c->getNext()) c->print();
}

void slabAllocator::destroyCache(kCache *cache) {
    if(cache==nullptr)return;
    for(kCache* c=head, *prev=nullptr; c!=nullptr; prev=c, c=c->getNext()){
        if(c==cache){
            if(prev!= nullptr){
                prev->setNext(c->getNext());
                kCache::destroyCache(c);
                kbuddyAllocator::free(c,sizeof(kCache));
                return;
            }
            else{
                head=c->getNext();
                kCache::destroyCache(c);
                kbuddyAllocator::free(c,sizeof(kCache));
                return;
            }
        }
    }
    kCache::destroyCache(cache);//cache not in list
    kbuddyAllocator::free(cache,sizeof(kCache));
    return;
}

kCache *slabAllocator::createCache(const char *name, size_t objSize, void (*ctor)(void *), void (*dtor)(void *)) {
    kCache* c = kCache::createCache(name, objSize, ctor, dtor);
    if(!c) return nullptr;//error, failed allocation of new cache
    c->setNext(head);
    head=c;
    return c;
}
