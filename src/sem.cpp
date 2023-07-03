#include "../h/sem.hpp"
#include "../h/printing.hpp"

int sem::id = 0;

int sem::wait() {
    var = var - 1;
    if(var < 0) {
        PCB::running->setBlocked(true);
        blocked.addLast(PCB::running);
        PCB::yield();
    }
    return 0;
}

int sem::signal() {
    var = var + 1;
    if (var <= 0 && blocked.peekFirst() != nullptr) {
        PCB* unblock = blocked.removeFirst();
        unblock->setBlocked(false);
        Scheduler::put(unblock);
    }
    return 0;
}

void sem::deleteSem() {
    while(blocked.peekFirst() != nullptr) {
        PCB* unblock = blocked.removeFirst();
        unblock->setBlocked(false);
        Scheduler::put(unblock);
    }
}

sem* sem::semOpen(int init) {
    return new sem(init);
}

sem::~sem() {
    deleteSem();
}

