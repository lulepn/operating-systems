#include "../h/syscall_cpp.hpp"

using size_t = decltype(sizeof(0));

void *operator new(size_t n) {
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p)  {
    mem_free(p);
}

void operator delete[](void *p) noexcept
{
    __mem_free(p);
}


Thread::Thread(void (*body)(void *), void *arg) {
    thread_create_norun(&myHandle,body,arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

void Thread::wrapper(void* ptr) {
    Thread* t = (Thread*)ptr;
    t->run();
}

int Thread::start() {
    if(myHandle != nullptr) {
        //myHandle->myPCB->put();
        scheduler_put(&myHandle);
    }
    else {
        thread_create(&myHandle, &wrapper, this);
    }
    return 0;
}

Thread::Thread() {
    myHandle = nullptr;
}

Thread::~Thread() {
    delete myHandle;
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle,init);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

void Console::putc(char c) {
    putcc(c);
}

char Console::getc() {
    return getcc();
}






