#include "../h/kCache.hpp"

size_t kCache::findOptimalSlabSize(size_t objSize) {
    int optimal=18;
    size_t minFrag=(1<<18)%objSize;
    size_t size=1<<12;
    for(int i=12;i<=18;i++,size*=2){
        if(size<objSize) continue;
        if(size%objSize<minFrag || (size%objSize==minFrag && i<optimal)){
            minFrag=size%objSize;
            optimal=i;
        }
    }
    return 1<<optimal;
}

kCache *kCache::createCache(const char *name, size_t objSize, void (*ctor)(void * ctorArgs), void (*dtor)(void *)) {
    kCache* c = (kCache*)kbuddyAllocator::alloc(sizeof(kCache));
    if(c==nullptr) return nullptr;//error, failed allocation of new cache
    for(int i=0;i<64;i++){
        if(name[i]=='\0') {
            c->name[i]='\0';
            break;
        }
        c->name[i]=name[i];
    }
    c->objSize=objSize;
    c->next= nullptr;
    c->ctor=ctor;
    c->dtor=dtor;
    c->fullSlabs=nullptr;
    c->halfFullSlabs= nullptr;
    c->emptySlabs=kSlab::createSlab(objSize, findOptimalSlabSize(objSize)/objSize);
    return c;
}

void *kCache::alloc() {
    void* ret = nullptr;
    kSlab* s=halfFullSlabs;
    if(s!= nullptr){
        ret = s->alloc();
        if(s->isFull()){
            halfFullSlabs=s->getNext();
            s->setNext(fullSlabs);
            fullSlabs=s;
        }
        if(ctor) ctor(ret);
        return ret;
    }
    s=emptySlabs;
    if(s!= nullptr){
        ret=s->alloc();
        emptySlabs=s->getNext();
        if(!s->isFull()){
            s->setNext(halfFullSlabs);
            halfFullSlabs=s;
        }
        else{
            s->setNext(fullSlabs);
            fullSlabs=s;
        }
        if(ctor) ctor(ret);
        return ret;
    }
    s=kSlab::createSlab(objSize, findOptimalSlabSize(objSize)/objSize);
    if(s!= nullptr){
        ret=s->alloc();
        if(!s->isFull()){
            s->setNext(halfFullSlabs);
            halfFullSlabs=s;
        }
        else{
            s->setNext(fullSlabs);
            fullSlabs=s;
        }
        if(ctor) ctor(ret);
        return ret;
    }
    return nullptr;//error, failed allocation of new slab
}

int kCache::free(void *ptr) {
    if(ptr== nullptr) return -1;
    int status=-1;
    for(kSlab* s=fullSlabs, *prev=nullptr; s!= nullptr; prev=s, s=s->getNext()){
        if(s->isHere(ptr)) {
            status = s->free(ptr);
            if (prev)prev->setNext(s->getNext());
            else fullSlabs = s->getNext();
            if (!s->isEmpty()) {
                s->setNext(halfFullSlabs);
                halfFullSlabs = s;
            } else {
                s->setNext(emptySlabs);
                emptySlabs = s;
                shrink();
            }
            if(dtor) dtor(ptr);
            return status;
        }
    }
    for(kSlab* s=halfFullSlabs, *prev=nullptr; s!= nullptr; prev=s, s=s->getNext()){
        if(s->isHere(ptr)) {
            status = s->free(ptr);
            if (s->isEmpty()) {
                if (prev)prev->setNext(s->getNext());
                else halfFullSlabs = s->getNext();
                s->setNext(emptySlabs);
                emptySlabs = s;
                shrink();
            }
            if(dtor) dtor(ptr);
            return status;
        }
    }
    return status;
}

void kCache::destroyCache(kCache* c) {
    if(c==nullptr) return;
    kSlab* s = nullptr, *tmp;
    s=c->emptySlabs;
    while (s!= nullptr){
        tmp=s;
        s=s->getNext();
        tmp->destroySlab();
        kbuddyAllocator::free(tmp,sizeof (kSlab));
    }
    c->emptySlabs= nullptr;
    s=c->halfFullSlabs;
    while (s!= nullptr){
        tmp=s;
        s=s->getNext();
        tmp->destroySlab();
        kbuddyAllocator::free(tmp,sizeof (kSlab));
    }
    c->halfFullSlabs= nullptr;
    s=c->fullSlabs;
    while (s!= nullptr){
        tmp=s;
        s=s->getNext();
        tmp->destroySlab();
        kbuddyAllocator::free(tmp,sizeof (kSlab));
    }
    c->fullSlabs= nullptr;
    c->setNext(nullptr);
    //kbuddyAllocator::free(c,sizeof(kCache));
    return;
}

bool kCache::isHere(void* ptr){
    if(ptr== nullptr) return false;
    for(kSlab* s=fullSlabs; s!= nullptr; s=s->getNext()) if(s->isHere(ptr)) return true;
    for(kSlab* s=halfFullSlabs; s!= nullptr; s=s->getNext()) if(s->isHere(ptr)) return true;
    return false;
}

void kCache::print() {
    kprintString("\nCache name: ");
    kprintString(name);
    kprintString("\nSlot size(B): ");
    kprintInteger(objSize);
    kprintString("\nCache size(Blocks(4096B)): ");
    int slabCnt=0;
    for(kSlab* s=fullSlabs;s!= nullptr;s=s->getNext()) slabCnt++;
    for(kSlab* s=halfFullSlabs;s!= nullptr;s=s->getNext()) slabCnt++;
    for(kSlab* s=emptySlabs;s!= nullptr;s=s->getNext()) slabCnt++;
    kprintInteger(1+slabCnt*(2+ (findOptimalSlabSize(objSize)/(1<<12))));
    kprintString("\nSlab count: ");
    kprintInteger(slabCnt);
    kprintString("\nNumber of slots in 1 slab: ");
    kprintInteger(findOptimalSlabSize(objSize)/objSize);
    kprintString("\nOccupancy percentage(0-100): ");
    int freeCnt=0;
    for(kSlab* s=fullSlabs;s!= nullptr;s=s->getNext()) freeCnt+=s->getNumOfFreeSlots();
    for(kSlab* s=halfFullSlabs;s!= nullptr;s=s->getNext()) freeCnt+=s->getNumOfFreeSlots();
    for(kSlab* s=emptySlabs;s!= nullptr;s=s->getNext()) freeCnt+=s->getNumOfFreeSlots();
    kprintInteger((slabCnt*(findOptimalSlabSize(objSize)/objSize)-freeCnt)*100/(slabCnt*(findOptimalSlabSize(objSize)/objSize)));
    kprintString("\n");
}

int kCache::shrink() { //dealocates all but 1 slab from empty slabs list
    while(emptySlabs){
        if(emptySlabs->getNext()){
            kSlab* tmp = emptySlabs->getNext();
            emptySlabs->setNext(emptySlabs->getNext()->getNext());
            tmp->destroySlab();
        }
        else break;
    }
    return 0;
}

