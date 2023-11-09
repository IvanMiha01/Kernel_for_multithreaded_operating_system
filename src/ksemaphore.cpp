#include "../h/ksemaphore.hpp"

extern "C" void pushRegisters();
extern "C" void popRegisters();

int kSemaphore::semWait(kSemaphore *sem) {
    if(sem == nullptr) return -1;
    if(--(sem->value) < 0){
        sem->block();
    }
    if(kTCB::running->blocked){
        kTCB::running->blocked = false;
        return -1;
    }
    return 0;
}

int kSemaphore::semSignal(kSemaphore *sem) {
    if(sem == nullptr) return -1;
    if(++(sem->value) <= 0){
        sem->unblock();
    }
    return 0;
}

int kSemaphore::semOpen(kSemaphore** handle, uint val) {
    if(handle == nullptr) return -1;
    //kSemaphore* sem = (kSemaphore*)kmemoryAllocator::mem_alloc((sizeof(kSemaphore) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    kSemaphore* sem = (kSemaphore*)slabAllocator::kSemaphoreCache->alloc();
    if (sem == 0) return 1;//error, failed allocation of new semaphore
    *handle = sem;
    sem->value = (int)val;
    sem->blocked = kTCBQueue();
    return 0;
}

int kSemaphore::semClose(kSemaphore *sem) {
    if(sem == nullptr) return -1;
    kTCB* tmp = (sem->blocked).get();
    while(tmp != nullptr){
        kScheduler::put(tmp);
        tmp = (sem->blocked).get();
    }
    //kmemoryAllocator::mem_free((void*)sem);
    slabAllocator::kSemaphoreCache->free(sem);
    return 0;
}

void kSemaphore::block() {
    kTCB* old = kTCB::running;
    blocked.put(old);
    old->blocked = true;
    kTCB::yield();
}

void kSemaphore::unblock() {
    kTCB* tmp = blocked.get();
    tmp->blocked = false;
    kScheduler::put(tmp);
}
