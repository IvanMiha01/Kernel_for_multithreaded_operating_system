#ifndef PROJECT_BASE_SEMAPHORE_HPP
#define PROJECT_BASE_SEMAPHORE_HPP

#include "../h/ktcbQueue.hpp"
#include "../h/kscheduler.hpp"

class kSemaphore {
public:
    static int semWait(kSemaphore* sem);

    static int semSignal(kSemaphore* sem);

    static int semOpen(kSemaphore** handle, uint val);

    static int semClose(kSemaphore* sem);
protected:
    void block();

    void unblock();
private:
    int value;
    kTCBQueue blocked;
};


#endif //PROJECT_BASE_SEMAPHORE_HPP
