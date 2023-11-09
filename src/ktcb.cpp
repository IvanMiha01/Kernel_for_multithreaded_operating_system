#include "../h/ktcb.hpp"
#include "../h/kmemoryAllocator.hpp"
#include "../h/kscheduler.hpp"

extern "C" void pushRegisters();
extern "C" void popRegisters();

kTCB* kTCB::running = nullptr;
uint kTCB::runningTimeSlice = 0;

void kTCB::threadWrapper() {
    kTCB::popSppSpie();
    kTCB::running->body(kTCB::running->arg);
    kTCB::threadExit();
}

int kTCB::threadCreate(kTCB** handle, kTCB::Body startRoutine, void *a, void *stackSpace) {
    //kTCB* thread = (kTCB*)kmemoryAllocator::mem_alloc((sizeof(kTCB) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    kTCB* thread = (kTCB*)slabAllocator::kTCBCache->alloc();
    if(thread == 0) return 1; //error, faild allocation of new thread
    if(handle)*handle = thread;
    thread->body = startRoutine;
    thread->arg = a;
    if(thread->body != nullptr){
        thread->stack = (uint64*)stackSpace;
        kScheduler::put(thread);
    }
    else{
        //kmemoryAllocator::mem_free(stackSpace);
        slabAllocator::stackCache->free(stackSpace);
        thread->stack = nullptr;
    }
    thread->context.ra = (uint64)&threadWrapper;
    thread->context.sp = (uint64)&(thread->stack[DEFAULT_STACK_SIZE]);
    thread->finished = false;
    thread->blocked = false;
    thread->started = false;
    thread->timeSilce = DEFAULT_TIME_SLICE;
    return 0;
}

void kTCB::dispatch() {
    kTCB *old = running;
    if (!old->isFinished()) {
        if(!old->blocked) kScheduler::put(old);
    }

    running = kScheduler::get();

    if(old->isFinished()){
        //kmemoryAllocator::mem_free((void*)old->stack);
        slabAllocator::stackCache->free(old->stack);
        //kmemoryAllocator::mem_free((void*)old);
        slabAllocator::kTCBCache->free(old);
    }

    kTCB::contextSwitch(&old->context, &running->context);
}

void kTCB::yield() {
    pushRegisters();

    dispatch();

    popRegisters();
}

int kTCB::threadExit() {
    if(kTCB::running == nullptr) return -1;
    kTCB::running->setFinished(true);
    kTCB::running->yield();
    return 0;
}

void kTCB::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

int kTCB::pausedThreadCreate(kTCB **handle, kTCB::Body startRoutine, void *a, void *stackSpace) {
    //kTCB* thread = (kTCB*)kmemoryAllocator::mem_alloc((sizeof(kTCB) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    kTCB* thread = (kTCB*)slabAllocator::kTCBCache->alloc();
    if(thread == 0) return 1; //error, faild allocation of new thread
    if(handle)*handle = thread;
    thread->body = startRoutine;
    thread->arg = a;
    if(thread->body != nullptr){
        thread->stack = (uint64*)stackSpace;
        //kScheduler::put(thread); //not ready, ready when start is called
    }
    else{
        //kmemoryAllocator::mem_free(stackSpace);
        slabAllocator::kfree(stackSpace);
        thread->stack = nullptr;
    }
    thread->context.ra = (uint64)&threadWrapper;
    thread->context.sp = (uint64)&(thread->stack[DEFAULT_STACK_SIZE]);
    thread->finished = false;
    thread->blocked = false;
    thread->started = false;
    thread->timeSilce = DEFAULT_TIME_SLICE;
    return 0;
}

int kTCB::start(kTCB *thread) {
    if(kTCB::running == nullptr) return -1;
    if(thread == nullptr) return -1;
    if(thread->started) return -1;
    thread->started = 1;
    kScheduler::put(thread);
    return 0;
}


