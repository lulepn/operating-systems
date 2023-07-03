

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "list.hpp"

class PCB;

class Scheduler
{
private:
    static List<PCB> readyQueue;

public:

    static PCB *peek();
    static PCB *get();

    static void put(PCB *pcb);

};

#endif //SCHEDULER_HPP
