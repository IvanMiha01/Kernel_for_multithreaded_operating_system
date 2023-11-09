#include "../h/kbuddyAllocator.hpp"


int kbuddyAllocator::freeChunks[25] = {-1,-1,-1,-1,-1,-1,-1,-1,
                                       -1,-1,-1,-1,-1,-1,-1,-1,
                                       -1,-1,-1,-1,-1,-1,-1,-1,0};

void kbuddyAllocator::init() {
    *((int*)HEAP_START_ADDR)=-1;
}

void *kbuddyAllocator::alloc(size_t size) {
    if(size>1<<24) return nullptr;//error, size is too big
    if(size<4096)size=4096;//smallest block is 1 page
    size_t tmp=1;
    int pow=0;
    while(tmp<size){
        pow++;
        tmp*=2;
    }
    if(freeChunks[pow]!=-1){
        int ret=freeChunks[pow];
        freeChunks[pow]=*((int*)((char*)HEAP_START_ADDR+ret));
        return (void*)((int*)((char*)HEAP_START_ADDR+ret));
    }
    else{
        for(int i=pow+1; i<maxPow+1; i++){
            int seg1=freeChunks[i];
            if(seg1>-1){
                int seg2=seg1+(1<<(i-1));
                int* pseg1=(int*)((char*)HEAP_START_ADDR+seg1);
                int* pseg2=(int*)((char*)HEAP_START_ADDR+seg2);

                freeChunks[i]=*pseg1;
                *pseg2=freeChunks[i-1];
                *pseg1=seg2;
                freeChunks[i-1]=seg1;

                return alloc(size);
            }
        }
    }
    return 0;//error, no space available
}

void kbuddyAllocator::free(void *addr, size_t size) {
    if(addr==nullptr) return;
    if(addr<HEAP_START_ADDR){
        kprintString("address to low for buddy allocator\n");
        return;
    }
    if(addr>((char*)HEAP_START_ADDR+(1<<24))){
        kprintString("address to high for buddy allocator\n");
        return;
    }
    if(size<4096) size = 4096;
    size_t tmp=1;
    int pow=0;
    while(tmp<size){
        pow++;
        tmp*=2;
    }
    int segx=((uint64)addr-(uint64)HEAP_START_ADDR);
    int segy;
    if(segx%(tmp*2)==0){
        segy=segx+(1<<pow);
    } else{
        segy=segx-(1<<pow);
    }
    int num = freeChunks[pow];
    int prev=-1;
    while(num>-1){
        if(num==segy){
            if(prev==-1){
                freeChunks[pow]=*((int*)((char*)HEAP_START_ADDR+segy));
            } else{
                *((int*)((char*)HEAP_START_ADDR+prev))=*((int*)((char*)HEAP_START_ADDR+segy));
            }
            return free((void*)((int*)((char*)HEAP_START_ADDR+(segx<segy?segx:segy))),1<<(pow+1));
        }
        prev=num;
        num=*((int*)((char*)HEAP_START_ADDR+num));
    }
    *((int*)((char*)HEAP_START_ADDR+segx))=freeChunks[pow];
    freeChunks[pow]=segx;
}

void kbuddyAllocator::print() {
    kprintString("\n---Buddy---\n");
    int x=1<<12;
    for(int i=12;i<maxPow+1;i++){
        kprintInteger(x);
        x*=2;
        kprintString(": ");
        int tmp = freeChunks[i];
        while(tmp>-1){
            kprintInteger(tmp);
            kprintString(", ");
            tmp=*((int*)((char*)HEAP_START_ADDR+tmp));
        }
        kprintInteger(tmp);
        kprintString("\n");
    }
}
