
#include "../h/scheduler.hpp"

List<PCB> Scheduler::readyQueue;

PCB *Scheduler::peek() {
    return readyQueue.peekFirst();
}

PCB *Scheduler::get()
{
    return readyQueue.removeFirst();
}

void Scheduler::put(PCB *pcb)
{
    readyQueue.addLast(pcb);
}