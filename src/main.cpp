#include "../h/syscall_cpp.hpp"
#include "../h/kmemoryAllocator.hpp"
#include "../h/kconsole.hpp"

extern "C" void supervisorTrap();

int noidle = 0;

void idle(void*){
    while (!noidle) {
        thread_dispatch();
    }
}

extern void userMain();

sem_t mainsem;

void userMainWrapper(void*){
    userRegime();
    userMain();
    sysRegime();
    sem_signal(mainsem);
}

int main(){
    __asm__ volatile("csrw stvec, %[vector]" : : [vector] "r" (&supervisorTrap));
    kmemoryAllocator::init();
    kbuddyAllocator::init();
    slabAllocator::init();
    kConsole::init();

    thread_t  mainThread;
    thread_create(&mainThread, nullptr, nullptr);
    kTCB::running = mainThread;

    sem_open(&mainsem, 0);

    thread_t  nt;
    thread_create(&nt, idle, nullptr);

    __asm__ volatile("csrs sstatus, 0x2"); //inte

    thread_create(nullptr, userMainWrapper, nullptr);
    sem_wait(mainsem);

    noidle = 1;
    return 0;
}
