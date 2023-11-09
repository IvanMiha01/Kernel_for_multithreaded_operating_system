#ifndef PROJECT_BASE_TCBQUEUE_HPP
#define PROJECT_BASE_TCBQUEUE_HPP

#include "klist.hpp"
#include "ktcb.hpp"

class kTCBQueue {
public:
    kTCB *get();

    void put(kTCB *tcb);
private:
    kList<kTCB> threadQueue;
};


#endif //PROJECT_BASE_TCBQUEUE_HPP
