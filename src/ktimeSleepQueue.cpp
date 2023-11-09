#include "../h/ktimeSleepQueue.hpp"
#include "../h/kmemoryAllocator.hpp"
#include "../h/kscheduler.hpp"

Elem* ktimeSleepQueue::head = nullptr;

int ktimeSleepQueue::put(uint64 val) {
    if(val == 0) return 0;
    //Elem* elem = (Elem*)kmemoryAllocator::mem_alloc((sizeof(Elem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    Elem* elem = (Elem*)slabAllocator::ktimeSleepQueueElemCache->alloc();
    if(elem == 0) return 1;//error, failed allocation of ktimeSleepQueueElem
    elem->data = kTCB::running;
    elem->time = val;
    elem->next = head;
    head = elem;
    kTCB::running->blocked = true;
    kTCB::yield();
    return 0;
}

void ktimeSleepQueue::updateTime() {
    for(Elem* cur = head; cur ; cur = cur->next){
        cur->time--;
    }
    ktimeSleepQueue::awake();
}

void ktimeSleepQueue::awake() {
    Elem* cur = head, * prev = nullptr;
    while(cur){
        if(cur->time == 0){
            cur->data->blocked = false;
            kScheduler::put(cur->data);
            if(prev) prev->next = cur->next;
            else head = cur->next;
            Elem* tmp = cur;
            cur = cur->next;
            //kmemoryAllocator::mem_free(tmp);
            slabAllocator::ktimeSleepQueueElemCache->free(tmp);
        }
        else{
            prev = cur;
            cur = cur->next;
        }
    }
}
