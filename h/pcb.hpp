
#ifndef PCB_HPP
#define PCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

// Thread Control Block
class PCB
{
public:
    static int stopRunning();

    ~PCB() { delete[] stack; }

    static int getNumT() { return numOfThr;}

    bool isFinished() const { return finished; }

    bool isBlocked() const { return blocked;}

    void setFinished(bool value) { finished = value; }

    void setBlocked(bool value) { blocked = value; }

    using Body = void (*)(void*);

    static PCB *createThread(Body body, void* arg, int mode);

    static void yield();

    static PCB *running;

    static int numOfThr;

    void put();


private:
    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    void* argument;
    Body body;
    uint64 *stack;
    Context context;
    bool finished;
    bool blocked;
    int tID;

    explicit PCB(Body body, void* arg, int mode) :
            body(body),
            stack(body != nullptr ? new uint64[DEFAULT_STACK_SIZE] : nullptr),
            context({body != nullptr ? (uint64) &threadWrapper : 0,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            finished(false),blocked(false)
    {
        if (body != nullptr && mode == 1) { Scheduler::put(this); }
        if (body != nullptr) {argument = arg;}
        tID = numOfThr;
    }

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void threadWrapper();

    static void dispatch();

};

#endif //PCB_HPP
