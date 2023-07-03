#ifndef _sem_hpp_
#define _sem_hpp_

#include "../h/pcb.hpp"
#include "../h/list.hpp"
#include "../h/scheduler.hpp"
#include "../lib/mem.h"

class PCB;

class sem {
    static int id;
    int idSem;
    int var;
    List<PCB> blocked;
public:
    int wait();
    int signal();
    int getID() { return idSem; }

    explicit sem() { var = 0; }

    explicit sem(int init) {
        var = init;
        id++;
        idSem = id;
    }
    static sem* semOpen(int init);
    void deleteSem();
    ~sem();
    void *operator new(size_t n)
    {
        return __mem_alloc(n);
    }

    void *operator new[](size_t n)
    {
        return __mem_alloc(n);
    }

    void operator delete(void *p) noexcept
    {
        __mem_free(p);
    }

    void operator delete[](void *p) noexcept
    {
        __mem_free(p);
    }
};


#endif