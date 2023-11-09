#include "../h/kconsole.hpp"

int kConsole::putHead = 0;
int kConsole::putTail = 0;
int kConsole::putSize = 0;
char* kConsole::putBuffer = nullptr;

int kConsole::getHead = 0;
int kConsole::getTail = 0;
int kConsole::getSize = 0;
char* kConsole::getBuffer = nullptr;

kSemaphore* kConsole::getSem = nullptr;

void kConsole::putchar(char c) {
    if(putSize == BUFFER_SIZE) return;
    putBuffer[putTail] = c;
    putTail = (putTail + 1) % BUFFER_SIZE;
    putSize++;
    kConsole::handle();
}

void kConsole::handle() {
    while(1){
        if(!(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT)) break;
        if(putSize == 0) break;
        *((char*)CONSOLE_TX_DATA) = putBuffer[putHead];
        putHead = (putHead + 1) % BUFFER_SIZE;
        putSize--;
    }
    while(1){
        if(!(*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT)) break;
        char tmp = *((char*)CONSOLE_RX_DATA);
        if(getSize == BUFFER_SIZE) continue;
        getBuffer[getTail] = tmp;
        getTail = (getTail + 1) % BUFFER_SIZE;
        getSize++;
        kSemaphore::semSignal(getSem);
    }
}

void kConsole::init() {
//    putBuffer = (char*)kmemoryAllocator::mem_alloc((kConsole::BUFFER_SIZE * sizeof(char) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
//    getBuffer = (char*)kmemoryAllocator::mem_alloc((kConsole::BUFFER_SIZE * sizeof(char)+ MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    putBuffer = (char*)slabAllocator::kalloc(BUFFER_SIZE);
    getBuffer = (char*)slabAllocator::kalloc(BUFFER_SIZE);
    kSemaphore::semOpen(&getSem, 0);
}

char kConsole::getchar() {
    //kConsole::handle();
    kSemaphore::semWait(getSem);
    char retval = getBuffer[getHead];
    getHead = (getHead + 1) % BUFFER_SIZE;
    getSize--;
    return retval;
}