#ifndef PROJECT_BASE_TIMESLEEPQUEUE_HPP
#define PROJECT_BASE_TIMESLEEPQUEUE_HPP

#include "../h/ktcb.hpp"

struct Elem
{
    kTCB* data;
    Elem *next;
    uint64 time;

    Elem(kTCB *data, Elem *next, uint64 time) : data(data), next(next), time(time) {}
};

class ktimeSleepQueue {
public:

    static int put(uint64 time);

    static void updateTime();

    static void awake();

private:
    static Elem* head;
};


#endif //PROJECT_BASE_TIMESLEEPQUEUE_HPP
