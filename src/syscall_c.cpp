#include "../h/syscall_c.h"
#include "../h/kmemoryAllocator.hpp"

void* mem_alloc (size_t size){
    size = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    __asm__ volatile("mv a1, %0": : "r" (size));
    __asm__ volatile("li a0, 0x01");

    __asm__ volatile("ecall");

    volatile uint64 retval = 0;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (void*)retval;
}

int mem_free(void* address){
    if(address== nullptr) return -1;
    auto addr = (uint64)address;

    __asm__ volatile("mv a1, %0": : "r" (addr));
    __asm__ volatile("li a0, 0x02");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){

    //void* stack = (void*)kmemoryAllocator::mem_alloc((DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    void* stack = (void*)slabAllocator::stackCache->alloc();

    if(stack == 0){
        *handle = 0;
        return 1;//error, failed allocation of stack
    }

    __asm__ volatile("mv a4, %0": : "r" (stack));
    __asm__ volatile("mv a3, %0": : "r" (arg));
    __asm__ volatile("mv a2, %0": : "r" (start_routine));
    __asm__ volatile("mv a1, %0": : "r" (handle));
    __asm__ volatile("li a0, 0x11");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int thread_exit(){
    __asm__ volatile("li a0, 0x12");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

void thread_dispatch(){
    __asm__ volatile("li a0, 0x13");

    __asm__ volatile("ecall");
}

int sem_open (sem_t* handle, unsigned init){
    __asm__ volatile("mv a2, %0": : "r" (init));
    __asm__ volatile("mv a1, %0": : "r" (handle));
    __asm__ volatile("li a0, 0x21");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int sem_close (sem_t handle){
    __asm__ volatile("mv a1, %0": : "r" (handle));
    __asm__ volatile("li a0, 0x22");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int sem_wait (sem_t id){
    __asm__ volatile("mv a1, %0": : "r" (id));
    __asm__ volatile("li a0, 0x23");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int sem_signal (sem_t id){
    __asm__ volatile("mv a1, %0": : "r" (id));
    __asm__ volatile("li a0, 0x24");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int time_sleep(time_t time){
    __asm__ volatile("mv a1, %0": : "r" (time));
    __asm__ volatile("li a0, 0x31");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

char getc(){
    __asm__ volatile("li a0, 0x41");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (char)retval;
}

void putc(char c){
    __asm__ volatile("mv a1, %0": : "r" (c));
    __asm__ volatile("li a0, 0x42");

    __asm__ volatile("ecall");
}

int pausedThread_create (thread_t* handle, void(*start_routine)(void*), void* arg){
    //void* stack = (void*)kmemoryAllocator::mem_alloc((DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    void* stack = (void*)slabAllocator::stackCache->alloc();

    if(stack == 0) {
        *handle = 0;
        return 1; //error, failed allocation of stack
    }

    __asm__ volatile("mv a4, %0": : "r" (stack));
    __asm__ volatile("mv a3, %0": : "r" (arg));
    __asm__ volatile("mv a2, %0": : "r" (start_routine));
    __asm__ volatile("mv a1, %0": : "r" (handle));
    __asm__ volatile("li a0, 0x14");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

int start(thread_t thread){
    if(thread==nullptr) return -1;
    __asm__ volatile("mv a1, %0": : "r" (thread));
    __asm__ volatile("li a0, 0x15");

    __asm__ volatile("ecall");

    volatile uint64 retval;

    __asm__ volatile("mv %[retval], a0" : [retval] "=r" (retval));

    return (int)retval;
}

void userRegime(){
    __asm__ volatile("li a0, 0x50");

    __asm__ volatile("ecall");
}

void sysRegime(){
    __asm__ volatile("li a0, 0x51");

    __asm__ volatile("ecall");
}
