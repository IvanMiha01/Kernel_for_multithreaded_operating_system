////
//// Created by os on 2/4/23.
////
//
//#include "../lib/hw.h"
//#include "../h/kprint.hpp"
//#include "../h/kbuddyAllocator.hpp"
//#include "../h/ktcb.hpp"
//#include "../h/ksemaphore.hpp"
//#include "../h/kSlab.hpp"
//#include "../h/kCache.hpp"
//#include "../h/slabAllocator.hpp"
//#include "../h/syscall_cpp.hpp"
//
//void test1(){
//    kbuddyAllocator::print();
//    void* ptr[5000];
//    for(int i=0;i<5000;i++){
//        ptr[i]=kbuddyAllocator::alloc(32*(i%5+1));
//        if(ptr[i]== nullptr) kprintInteger(i);
//    }
//    kbuddyAllocator::print();
//    for(int i=0;i<5000;i++){
//        kbuddyAllocator::free(ptr[i],32*(i%5+1));
//    }
//    kbuddyAllocator::print();
//}
//
//void test2(){
//    kbuddyAllocator::print();
//    void* p1=kbuddyAllocator::alloc(128);
//    void* p2=kbuddyAllocator::alloc(256);
//    void* p3=kbuddyAllocator::alloc(128);
//    void* p4=kbuddyAllocator::alloc(128);
//    kbuddyAllocator::print();
//    kbuddyAllocator::free(p1,128);
//    kbuddyAllocator::free(p2,256);
//    kbuddyAllocator::free(p3,128);
//    kbuddyAllocator::free(p4,128);
//    kbuddyAllocator::print();
//}
//
//size_t findOptimalSlabSize(size_t objSize){
//    int optimal=18;
//    size_t minFrag=(1<<18)%objSize;
//    size_t size=1<<12;
//    for(int i=12;i<=18;i++,size*=2){
//        if(size<objSize) continue;
//        if(size%objSize<minFrag || (size%objSize==minFrag && i<optimal)){
//            minFrag=size%objSize;
//            optimal=i;
//        }
//        kprintInteger(i);
//        kprintString(" - ");
//        kprintInteger(size%objSize);
//        kprintString("\n");
//    }
//    kprintInteger(1<<optimal);
//    kprintString(" - optimal\n");
//    return 1<<optimal;
//}
//
//void test3(){
//    kprintString("sizeof kTCB: ");
//    kprintInteger(sizeof(kTCB));
//    kprintString("\noptimal bucket size\n");
//    kprintInteger(findOptimalSlabSize(sizeof(kTCB)));
//    kprintString("\n");
//    kprintString("sizeof kSemaphore: ");
//    kprintInteger(sizeof(kSemaphore));
//    kprintString("\noptimal bucket size\n");
//    kprintInteger(findOptimalSlabSize(sizeof(kSemaphore)));
//    kprintString("\n");
//}
//
//void test4(){
//    kprintInteger(sizeof (kTCB));
//    kprintString(" - velicina kTCB\n");
//    kbuddyAllocator::print();
//    kSlab* s = kSlab::createSlab(sizeof(kTCB), findOptimalSlabSize(sizeof(kTCB))/sizeof(kTCB));
//    kTCB* ptrs[100];
//    int i;
//    for(i=0;i<100;i++){
//        ptrs[i]=(kTCB*)s->alloc();
//        if(ptrs[i]==nullptr) {
//            kprintString("nema vise mesta - ");
//            kprintInteger(i);
//            break;
//        }
//    }
//    if(i==100){
//        kprintString("stalo sve\n");
//        i--;
//    }
//
//    for (;i>=0;i--){
//        s->free(ptrs[i]);
//    }
//    s->destroySlab();
//    kprintString("na kraju\n");
//    kbuddyAllocator::print();
//}
//
//void test5(){
//    kbuddyAllocator::print();
//    kCache* c = kCache::createCache("TBC_Cache",sizeof (kTCB), nullptr, nullptr);
//    kbuddyAllocator::print();
//    const int num=250;
//    kTCB* ptrs[num];
//    for(int i=0;i<250;i++){
//        ptrs[i]=(kTCB*)c->alloc();
//    }
//    ptrs[1]=ptrs[1];
//    c->print();
//    c->destroyCache();
//    kbuddyAllocator::print();
//}
//
//void test6(){
//    char* ptrs[100];
//    for(int i=0;i<100;i++){
//        ptrs[i] = (char*)slabAllocator::kalloc(1<<5<<(i%8));
//    }
//    slabAllocator::print();
//    kprintString("------------------------------------------");
//    slabAllocator::kfree(ptrs[7]);
//    slabAllocator::kfree(ptrs[15]);
//    slabAllocator::kfree(ptrs[23]);
//    slabAllocator::kfree(ptrs[31]);
//    slabAllocator::print();
//}
//
//void test7(){
//    while (kbuddyAllocator::alloc(4096)!=nullptr);
//    kbuddyAllocator::print();
//}
//
//void test8(){
//    kprintString("nesto sa mem_alloc: ");
//    kprintInteger((uint64)mem_alloc(150));
//    kprintString("\n");
//    kbuddyAllocator::print();
//}
//
//void testxxx(){
//    kprintString("--------1---------");
//    kbuddyAllocator::print();
//    void* p1=kbuddyAllocator::alloc(4096);
//    kprintString("--------2---------");
//    kbuddyAllocator::print();
//    void* p2=kbuddyAllocator::alloc(4096);
//    kprintString("--------3---------");
//    kbuddyAllocator::print();
//    void* p3=kbuddyAllocator::alloc(4096);
//    kprintString("--------4---------");
//    kbuddyAllocator::print();
//    kbuddyAllocator::free(p1,4096);
//    kprintString("--------5---------");
//    kbuddyAllocator::print();
//    kbuddyAllocator::free(p2,4096);
//    kprintString("--------6---------");
//    kbuddyAllocator::print();
//    kbuddyAllocator::free(p3,4096);
//    kprintString("--------7---------");
//    kbuddyAllocator::print();
//}
//
//void userMain() {
//    kprintString("Velicina hipa: ");
//    kprintInteger(((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / 8);
//    kprintString(" bajtova\n");
//    //16773506 skoro 2^24 (16777216)
//    kprintInteger((uint64)HEAP_START_ADDR);
//    kprintString("-");
//    kprintInteger((uint64)HEAP_END_ADDR);
//    kprintString("\n");
//
//    test1();
//}