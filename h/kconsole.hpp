#ifndef PROJECT_BASE_CONSOLE_HPP
#define PROJECT_BASE_CONSOLE_HPP

#include "../lib/hw.h"
#include "../h/kmemoryAllocator.hpp"
#include "../h/slabAllocator.hpp"
#include "../h/ksemaphore.hpp"

class kConsole {
public:
    static void init();

    static void putchar(char c);

    static char getchar();

    static void handle();
private:
    static const int BUFFER_SIZE = 1024;

    static kSemaphore* getSem;

    static char* putBuffer;
    static int putHead, putTail, putSize;

    static char* getBuffer;
    static int getHead, getTail, getSize;

};


#endif //PROJECT_BASE_CONSOLE_HPP
