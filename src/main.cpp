#include "../lib/hw.h"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/printing.hpp"

void userMain();

int main() {
    Riscv::w_stvec((uint64) &Riscv::interruptVector); // postavljanje prekidne rutine
    // menjanje rezima u korisnicki
    int code = 100;
    __asm__ volatile("mv a0, %[kod]" : : [kod] "r" (code));
    __asm__ volatile("ecall");

    PCB* mainThread = PCB::createThread(nullptr, nullptr, 0);
    PCB::running = mainThread;

    userMain();

    return 0;
}




