#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"

void* mem_alloc (size_t size) {
    Riscv::syscall(1,&size,nullptr,nullptr, nullptr);
    void* ret;
    __asm__ volatile("mv %[kod], a1" : [kod] "=r" (ret));
    return ret;
}

int mem_free (void* p) {
    Riscv::syscall(2,p,nullptr,nullptr, nullptr);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

void scheduler_put(thread_t* handle) {
    Riscv::syscall(15,handle,nullptr, nullptr, nullptr);
}

int thread_create_norun(thread_t* handle, void(*start_routine)(void*), void* arg) {
    *handle = new _thread;
    thread_t hand = *handle;
    Riscv::syscall(16,hand,arg,nullptr, start_routine);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    *handle = new _thread;
    thread_t hand = *handle;
    Riscv::syscall(17,hand,arg,nullptr, start_routine);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

int thread_exit() {
    Riscv::syscall(18,nullptr,nullptr,nullptr, nullptr);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

void thread_dispatch() {
    Riscv::syscall(19,nullptr,nullptr,nullptr, nullptr);
}

int sem_open (sem_t* handle, unsigned int init) {
    *handle = new _sem;
    sem_t hand = *handle;
    Riscv::syscall(33,hand,&init,nullptr, nullptr);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

int sem_close (sem_t handle) {
    Riscv::syscall(34,handle,nullptr,nullptr, nullptr);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

int sem_wait (sem_t id) {
    Riscv::syscall(35,id,nullptr,nullptr, nullptr);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

int sem_signal (sem_t id) {
    Riscv::syscall(36,id,nullptr,nullptr, nullptr);
    int ret;
    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (ret));
    return ret;
}

char getc() {
    Riscv::syscall(65,nullptr,nullptr,nullptr, nullptr);
    char ret;
    __asm__ volatile("mv %[kod], a1" : [kod] "=r" (ret));
    return ret;
}

char getcc() {
    return getc();
}

void putc(char c) {
    void* ptr;
    ptr = &c;
    Riscv::syscall(66,ptr,nullptr,nullptr, nullptr);
}

void putcc(char c) {
    putc(c);
}