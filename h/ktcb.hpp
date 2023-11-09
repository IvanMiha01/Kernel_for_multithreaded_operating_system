#ifndef PROJECT_BASE_TCB_HPP
#define PROJECT_BASE_TCB_HPP


#include "../lib/hw.h"

class kTCB {
public:
    static void popSppSpie();

    using Body = void (*)(void*);

    void setFinished(bool value){
        finished = value;
    }

    bool isFinished() const{
        return finished;
    }

    uint64 getTimeSlice() const{
        return timeSilce;
    }

    static kTCB* running;

    static uint runningTimeSlice;

    static int threadCreate(kTCB** handle, Body startRoutine, void* arg, void* stackSpace);

    static void yield();

    static int threadExit();

    static int pausedThreadCreate(kTCB** handle, Body startRoutine, void* arg, void* stackSpace);

    static int start(kTCB* thread);
private:

    struct Context{
        uint64 ra;
        uint64 sp;
    };
    Context context;
    uint64* stack;
    Body body;
    bool finished;
    bool blocked;
    bool started;
    void* arg;
    uint timeSilce;

    friend class kSemaphore;

    friend class ktimeSleepQueue;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void threadWrapper();

    static void dispatch();
};


#endif //PROJECT_BASE_TCB_HPP
