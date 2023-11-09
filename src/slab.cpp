#include "../h/slab.h"
#include "../lib/hw.h"

void kmem_init(void *space, int block_num){
    return;
}
kmem_cache_t *kmem_cache_create(const char *name, size_t size, void (*ctor)(void *), void (*dtor)(void *)){
    return slabAllocator::createCache(name, size, ctor, dtor);
}// Allocate cache

int kmem_cache_shrink(kmem_cache_t *cachep){
    if(cachep==nullptr) return -1;
    return cachep->shrink();
} // Shrink cache

void *kmem_cache_alloc(kmem_cache_t *cachep){
    if(cachep==nullptr) return nullptr;
    return cachep->alloc();
} // Allocate one object from cache

void kmem_cache_free(kmem_cache_t *cachep, void *objp){
    if(cachep==nullptr || objp== nullptr) return;
    cachep->free(objp);
} // Deallocate one object from cache

void *kmalloc(size_t size){
    return slabAllocator::kalloc(size);
} // Alloacate one small memory buffer

void kfree(const void *objp){
    if(objp==nullptr) return;
    slabAllocator::kfree((void*)objp);
} // Deallocate one small memory buffer

void kmem_cache_destroy(kmem_cache_t *cachep){
    if(cachep== nullptr) return;
    slabAllocator::destroyCache(cachep);
} // Deallocate cache

void kmem_cache_info(kmem_cache_t *cachep){
    if(cachep== nullptr) return;
    cachep->print();
} // Print cache info

int kmem_cache_error(kmem_cache_t *cachep){
    if(cachep== nullptr) return -1;
    return 0;
} // Print error message