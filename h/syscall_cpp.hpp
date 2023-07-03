#ifndef _syscall_cpp
#define _syscall_cpp
#include "syscall_c.hpp"

void* operator new (size_t);

void operator delete (void*);

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();

protected:
    Thread ();
    virtual void run () {}
private:
    static void wrapper(void* v);
    thread_t myHandle;
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};
#endif