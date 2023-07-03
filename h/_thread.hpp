#ifndef __thread_hpp_
#define __thread_hpp_

#include "../h/pcb.hpp"

class _thread {
public:
    PCB* myPCB;
    _thread() { myPCB = nullptr;}
};


#endif