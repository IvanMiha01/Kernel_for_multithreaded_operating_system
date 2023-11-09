#include "../h/ktcbQueue.hpp"

kTCB *kTCBQueue::get() {
    return threadQueue.removeFirst();
}

void kTCBQueue::put(kTCB *tcb) {
    threadQueue.addLast(tcb);
}
