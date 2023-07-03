#ifndef _syscalls_c_hpp_
#define _syscalls_c_hpp_
#include "../h/_sem.hpp"
#include "../h/_thread.hpp"
#include "../lib/mem.h"


void* mem_alloc (size_t size);

int mem_free (void* p);

class _thread;

typedef _thread* thread_t;

void scheduler_put(thread_t* handle);

int thread_create_norun(thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit ();

void thread_dispatch ();

class _sem;

typedef _sem* sem_t;

int sem_open (sem_t* handle, unsigned int init);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

const int EOF = -1;

char getc ();

char getcc();

void putc (char);

void putcc(char);

#endif

