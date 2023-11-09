#ifndef PROJECT_BASE_SYSCALL_C_HPP
#define PROJECT_BASE_SYSCALL_C_HPP

#include "../lib/hw.h"
#include "../h/ktcb.hpp"
#include "../h/ksemaphore.hpp"

void* mem_alloc (size_t size);

int mem_free (void*);

typedef kTCB* thread_t;

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

void thread_dispatch();

int thread_exit();

typedef kSemaphore* sem_t;

int sem_open (sem_t* handle, unsigned init);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

typedef unsigned long time_t;

int time_sleep(time_t);

char getc();

void putc(char c);

int pausedThread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

int start(thread_t thread);

void userRegime();

void sysRegime();

#endif //PROJECT_BASE_SYSCALL_C_HPP
