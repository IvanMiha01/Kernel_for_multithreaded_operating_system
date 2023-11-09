#ifndef PROJECT_BASE_LIST_HPP
#define PROJECT_BASE_LIST_HPP

//#include "../h/kmemoryAllocator.hpp"
#include "../h/slabAllocator.hpp"

template<typename T>
class kList
{
public:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}
    };
private:

    Elem *head, *tail;

public:


    kList() : head(0), tail(0) {}

    //kList(const kList<T> &) = delete;

    //kList<T> &operator=(const kList<T> &) = delete;

    void addFirst(T *data)
    {
        //Elem *elem = new Elem(data, head);
        //Elem* elem = (Elem*)kmemoryAllocator::mem_alloc(sizeof(Elem));
        //Elem* elem = (Elem*)kmemoryAllocator::mem_alloc((sizeof(Elem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
        Elem* elem = (Elem*)slabAllocator::kListElemCache->alloc();
        if(elem == 0) return;//error, failed allocation of new kListElem
        elem->data = data;
        elem->next = head;
        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data)
    {
        //Elem *elem = new Elem(data, 0);
        //Elem* elem = (Elem*)kmemoryAllocator::mem_alloc(sizeof(Elem));
        //Elem* elem = (Elem*)kmemoryAllocator::mem_alloc((sizeof(Elem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
        Elem* elem = (Elem*)slabAllocator::kListElemCache->alloc();
        if(elem == 0) return;//error, failed allocation of new kListElem
        elem->data = data;
        elem->next = 0;
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    T *removeFirst()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        //delete elem;
        //kmemoryAllocator::mem_free(elem);
        slabAllocator::kListElemCache->free(elem);
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        //delete elem;
        //kmemoryAllocator::mem_free(elem);
        slabAllocator::kListElemCache(elem);
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }
};

#endif //PROJECT_BASE_LIST_HPP
