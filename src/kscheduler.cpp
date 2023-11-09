#include "../h/kscheduler.hpp"

kList<kTCB> kScheduler::readyThreadQueue;

kTCB *kScheduler::get()
{
    return readyThreadQueue.removeFirst();
}

void kScheduler::put(kTCB *tcb)
{
    readyThreadQueue.addLast(tcb);
}