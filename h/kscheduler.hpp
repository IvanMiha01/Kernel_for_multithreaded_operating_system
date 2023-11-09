#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

#include "klist.hpp"

class kTCB;

class kScheduler
{
private:
    static kList<kTCB> readyThreadQueue;

public:
    static kTCB *get();

    static void put(kTCB *tcb);

};

#endif //PROJECT_BASE_SCHEDULER_HPP
