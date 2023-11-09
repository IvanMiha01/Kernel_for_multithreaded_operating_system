#include "../h/kmemoryAllocator.hpp"
#include "../h/kprint.hpp"

FreeMem* kmemoryAllocator::head = nullptr;

void kmemoryAllocator::init() {
    if(head == nullptr){
        head = (FreeMem*)((char*)HEAP_START_ADDR+(1<<24));
        head->next = nullptr;
        head->size = (size_t)HEAP_END_ADDR - (size_t)((char*)HEAP_START_ADDR+(1<<24)) - 1 - sizeof(FreeMem);
    }
}

void *kmemoryAllocator::mem_alloc(size_t blocks) {
    //printString("alloc\n");

    if(blocks == 0) blocks++;

    size_t size = blocks * MEM_BLOCK_SIZE;
    for(FreeMem* cur = head, *prev = nullptr; cur != nullptr; prev = cur, cur = cur->next){
        if(cur->size < size) continue;
        if(cur->size - size < sizeof(FreeMem)){
            if(prev) prev->next = cur->next;
            else head = cur->next;
            cur->next = nullptr;
            return (void*)((char*)cur+sizeof(FreeMem));
        }
        else{
            FreeMem* newFreeMem = (FreeMem*)((char*)cur + size + sizeof(FreeMem));
            newFreeMem->next = cur->next;
            newFreeMem->size = cur->size - size - sizeof(FreeMem);
            if(prev) prev->next = newFreeMem;
            else head = newFreeMem;
            cur->size = size;
            cur->next = nullptr;
            return (void*)((char*)cur + sizeof(FreeMem));
        }
    }
    //printString("no space\n");
    //kprintString("no space\n");
    return nullptr;
}

int tryToJoin(FreeMem* cur){
    if(!cur) return 0;
    if(cur->next && ((uint64)cur + cur->size + sizeof(FreeMem)) == (uint64)(cur->next)){
        cur->size += cur->next->size + sizeof(FreeMem);
        cur->next = cur->next->next;
        return 1;
    }
    return 0;
}

int kmemoryAllocator::mem_free(void *address) {
    if(address == nullptr) return 0;
    //printString("free\n");
    FreeMem* cur;
    if(!head || address < (char*)head){
        cur = nullptr;
    }
    else{
        for(cur = head; cur->next != nullptr && address > (char*)cur->next; cur = cur->next);
    }
    FreeMem* newFreeMem = (FreeMem*)((char*)address - sizeof(FreeMem));
    if(cur){
        newFreeMem->next = cur->next;
        cur->next = newFreeMem;
    }
    else{
        newFreeMem->next = head;
        head = newFreeMem;
    }
    if(tryToJoin(newFreeMem)) ;//printString("join after\n");
    if(tryToJoin(cur)) ;//printString("join before\n");
    return 0;
}

size_t kmemoryAllocator::checkSize() {
    size_t sum = 0;
    for(FreeMem* cur = head; cur != nullptr; cur = cur->next) sum+=cur->size;
    return sum;
}
