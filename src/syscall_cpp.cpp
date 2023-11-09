#include "../h/syscall_cpp.hpp"

Thread::Thread(void (*body)(void *), void *arg) {
    ::pausedThread_create(&myHandle, body, arg);
}

Thread::~Thread() {
    //::thread_exit();
}

int Thread::start() {
    if(myHandle== nullptr) return -1;
    return ::start(myHandle);
}

void Thread::dispatch() {
    ::thread_dispatch();
}

int Thread::sleep(time_t t) {
    return ::time_sleep(t);
}

Thread::Thread() {
    ::pausedThread_create(&myHandle, wrapper, this);
}

void Thread::wrapper(void *t) {
    if(t)((Thread*)t)->run();
}

Semaphore::Semaphore(unsigned int init) {
    ::sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    ::sem_close(myHandle);
}

int Semaphore::wait() {
    return ::sem_wait(myHandle);
}

int Semaphore::signal() {
    return ::sem_signal(myHandle);
}

PeriodicThread::Elem* PeriodicThread::head = nullptr;

PeriodicThread::PeriodicThread(time_t period) : Thread(wrapper, this){
    PeriodicThread::head = new Elem(PeriodicThread::head, this, period);
}

void PeriodicThread::wrapper(void *pt) {
    if(!pt){
        putc('r');
        return;
    }
    Elem* cur;
    while(1){
        ((PeriodicThread*)pt)->periodicActivation();
        for(cur = head; cur->pt != pt; cur = cur->next);
        sleep(cur->period);
    }
}


char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}