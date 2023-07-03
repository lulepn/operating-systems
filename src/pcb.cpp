#include "../h/pcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

PCB *PCB::running = nullptr;

int PCB::numOfThr = 0;

int PCB::stopRunning() {
    if(Scheduler::peek() != nullptr) {
        PCB::running->setFinished(true);
        PCB::yield();
        return 1;
    } else {
        return 5;
    }
}

PCB *PCB::createThread(Body body, void* arg, int mode)
{
    numOfThr++;
    return new PCB(body, arg, mode);
}

void PCB::put() {
    Scheduler::put(this);
}

void PCB::yield()
{
    Riscv::pushRegisters();


    PCB::dispatch();


    Riscv::popRegisters();
}

void PCB::dispatch()
{
    PCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();

    PCB::contextSwitch(&old->context, &running->context);
}

void PCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->argument);
    running->setFinished(true);
    thread_dispatch();
}


