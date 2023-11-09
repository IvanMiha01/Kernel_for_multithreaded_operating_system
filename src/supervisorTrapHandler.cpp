#include "../lib/hw.h"
#include "../h/kmemoryAllocator.hpp"
#include "../h/ktcb.hpp"
#include "../h/ksemaphore.hpp"
#include "../h/ktimeSleepQueue.hpp"
#include "../h/kconsole.hpp"
#include "../h/kprint.hpp"

extern "C" void handleSupervisorTrap(){
    //trap

    uint64 a0, a1, a2, a3, a4, a5, a6, a7;
    __asm__ volatile("sd a7, %0":"=m"(a7));
    __asm__ volatile("sd a6, %0":"=m"(a6));
    __asm__ volatile("sd a5, %0":"=m"(a5));
    __asm__ volatile("sd a4, %0":"=m"(a4));
    __asm__ volatile("sd a3, %0":"=m"(a3));
    __asm__ volatile("sd a2, %0":"=m"(a2));
    __asm__ volatile("sd a1, %0":"=m"(a1));
    __asm__ volatile("sd a0, %0":"=m"(a0));

    uint64 sscratch;
    __asm__ volatile("csrr %0, sscratch":"=r"(sscratch));

    uint64 scause;
    __asm__ volatile("csrr %0, scause" : "=r" (scause));

    uint64 sstatus;
    __asm__ volatile("csrr %0, sstatus" : "=r" (sstatus));

    if(scause == 0x9UL || scause == 0x8UL){
        //ecall

        uint64 sepc;
        __asm__ volatile("csrr %0, sepc" : "=r" (sepc));

        if(a0 == 0x01){
            //mem alloc
            void* retval = kmemoryAllocator::mem_alloc(a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x02){
            //mem free
            int retval = kmemoryAllocator::mem_free((void*)a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x11){
            //thread create
            int retval = kTCB::threadCreate((kTCB**)a1, (kTCB::Body)a2, (void*)a3, (void*)a4);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x12){
            //thread exit
            int retval = kTCB::threadExit();
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x13){
            //thread dispatch
            kTCB::runningTimeSlice = 0;
            kTCB::yield();
        }
        else if(a0 == 0x14){
            //paused thread create
            int retval = kTCB::pausedThreadCreate((kTCB**)a1, (kTCB::Body)a2, (void*)a3, (void*)a4);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x15){
            //start
            int retval = kTCB::start((kTCB*)a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x21){
            //sem open
            int retval = kSemaphore::semOpen((kSemaphore**)a1, (uint)a2);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x22){
            //sem close
            int retval = kSemaphore::semClose((kSemaphore*)a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x23){
            //sem wait
            int retval = kSemaphore::semWait((kSemaphore*)a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x24){
            //sem signal
            int retval = kSemaphore::semSignal((kSemaphore*)a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x31){
            //time sleep
            kTCB::runningTimeSlice = 0;
            int retval = ktimeSleepQueue::put((uint64)a1);
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x41){
            //getc
            char retval = kConsole::getchar();
            __asm__ volatile("mv a0, %0": :"r" (retval));
        }
        else if(a0 == 0x42){
            //putc
            kConsole::putchar((uint64)a1);
        }
        else if(a0 == 0x50){
            //user regime
            sstatus = sstatus & (~0x100);
        }
        else if(a0 == 0x51){
            //sys regime
            sstatus = sstatus | (0x100);
        }

        __asm__ volatile("csrw sstatus, %0": : "r" (sstatus));

        __asm__ volatile("csrw sscratch, %0": : "r" (sscratch));

        //offset 0x50 = 80 dec = 10th register on stack, x10 <=> a0 return value
        __asm__ volatile("sd a0,0x50(%0)" : : "r"(sscratch));

        //adjust sepc - add 4 (ecall saves its address, not the address of the next instruction)
        sepc += 4;
        __asm__ volatile("csrw sepc, %0" : : "r" (sepc));
    }
    else if(scause == (0x01UL << 63 | 0x01UL)){
        //software interrupt - timer
        kTCB::runningTimeSlice++;
        ktimeSleepQueue::updateTime();
        if(kTCB::runningTimeSlice >= kTCB::running->getTimeSlice()){
            kTCB::runningTimeSlice = 0;
            uint64 sepc;
            __asm__ volatile("csrr %0, sepc" : "=r" (sepc));
            kTCB::yield();
            __asm__ volatile("csrw sstatus, %0": : "r" (sstatus));
            __asm__ volatile("csrw sepc, %0" : : "r" (sepc));
        }
        //software interrupt handled
        __asm__ volatile("csrc sip, 0x02");
    }
    else if(scause == (0x01UL << 63 | 0x09UL)){
        //hardware interrupt
        int flag = plic_claim();
        if(flag == CONSOLE_IRQ) {
            kConsole::handle();
        }
        plic_complete(flag);
    }
    else{
        //unexpected scause

        //undefined behaviour

        //print(scause)
        kprintString("\nscause: ");
        kprintInteger(scause);

        //print(sepc)
        uint64 sepc;
        __asm__ volatile("csrr %0, sepc" : "=r" (sepc));
        kprintString("\nsepc: ");
        kprintInteger(sepc);

        //print(sstatus)
        kprintString("\nsstatus: ");
        kprintInteger(sstatus);

        for(int i=0;i<30000;i++){
            for(int j=0;j<10000;j++);
        }
    }
}