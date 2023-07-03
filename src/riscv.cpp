#include "../h/pcb.hpp"
#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/syscall_c.hpp"

using Body = void (*)(void*);

void Riscv::handleInterrupt() {
    uint64 scause = Riscv::r_scause();
    uint64 volatile retA0 = 1;
    void* volatile retA1 = nullptr;
    char volatile retA11;
    uint64 volatile code; // citanje koda sistemskog poziva

    __asm__ volatile("mv %[kod], a0" : [kod] "=r" (code)); // kod SP iz A0 u promenljivu Code

    if (scause == 0x0000000000000008UL) {
        // nije prekid, ecall iz U moda
        uint64 volatile sepc = Riscv::r_sepc() + 4;
        uint64 volatile sstatus = Riscv::r_sstatus();
        mc_sstatus(SSTATUS_SIE);

        if (code == 1) {
            //void* mem_alloc (size_t size);
            size_t* tmp;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (tmp));
            retA1 = __mem_alloc(*tmp);
        }
        else if (code == 2) {
            //int mem_free (void*);
            void* tmp;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (tmp));
            retA0 = __mem_free(tmp);
        }
        else if (code == 15) {
            thread_t* handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            (*handle)->myPCB->put();
        }
        else if (code == 16) {
            // int thread_create_norun (thread_t* handle, void(*start_routine)(void*), void* arg);
            void* arg;
            Body routine;
            thread_t handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            __asm__ volatile("mv %[kod], a2" : [kod] "=r" (routine));
            __asm__ volatile("mv %[kod], a3" : [kod] "=r" (arg));
            handle->myPCB = PCB::createThread(routine,arg,0);
            if(handle->myPCB != nullptr) {
                retA0 = 1;
            }
            else {
                retA0 = -1;
            }
        }
        else if (code == 17) {
            // int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);
            void* arg;
            Body routine;
            thread_t handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            __asm__ volatile("mv %[kod], a2" : [kod] "=r" (routine));
            __asm__ volatile("mv %[kod], a3" : [kod] "=r" (arg));
            handle->myPCB = PCB::createThread(routine,arg,1);
            if(handle->myPCB != nullptr) {
                retA0 = 1;
            }
            else {
                retA0 = -1;
            }
        }
        else if (code == 18) {
            // int thread_exit();
            retA0 = PCB::stopRunning();
        }
        else if (code == 19) {
            // void thread_dispatch();
            PCB::yield();
        }
        else if (code == 33) {
            // int sem_open (sem_t* handle, unsigned init);
            int* init;
            sem_t handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            __asm__ volatile("mv %[kod], a2" : [kod] "=r" (init));
            handle->mySEM = sem::semOpen(*init);
            if(handle->mySEM != nullptr) {
                retA0 = 1;
            }
            else{
                retA0 = -1;
            }

        }
        else if (code == 34) {
            // int sem_close(sem_t handle);
            sem_t handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            delete handle->mySEM;
            handle->mySEM = nullptr;

            retA0 = 1;
        }
        else if (code == 35) {
            // int sem_wait (sem_t id);
            sem_t handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            handle->mySEM->wait();

            retA0 = 1;

        }
        else if (code == 36) {
            // int sem_signal (sem_t id);
            sem_t handle;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (handle));
            handle->mySEM->signal();

            retA0 = 1;

        }
        else if (code == 65) {
            //char getc();
            uint64 sstatus_tmp = r_sstatus();
            Riscv::w_stvec((uint64) &Riscv::handleConsoleTrap);
            retA11 = __getc();
            w_sstatus(sstatus_tmp);
            Riscv::w_stvec((uint64) &Riscv::interruptVector);

        }
        else if (code == 66) {
            //void putc(char c);
            char* tmp;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (tmp));
            __putc(*tmp);
        }
        Riscv::w_sstatus(sstatus);
        Riscv::w_sepc(sepc);

    }

    else if (scause == 0x0000000000000009UL) {
        // nije prekid, ecall iz S moda
        uint64 volatile sepc = Riscv::r_sepc() + 4;
        if (code == 1) {
            //void* mem_alloc (size_t size);
            size_t* tmp;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (tmp));
            retA1 = __mem_alloc(*tmp);
            Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
        }
        else if (code == 2) {
            //void mem_free(void*);
            void* tmp;
            __asm__ volatile("mv %[kod], a1" : [kod] "=r" (tmp));
            retA0 = __mem_free(tmp);
            Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
        }
        else if (code == 100) {
            Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
        }

        Riscv::w_sepc(sepc);

    }

    else if (scause == 0x8000000000000009UL) {
        // prekid, konzola
        console_handler();
    }
    else if (scause == 0x8000000000000001UL) {
        // prekid, tajmer
        Riscv::mc_sip(Riscv::SIP_SSIP);
    }

    __asm__ volatile("mv a0, %[kod]" : : [kod] "r" (retA0));
    if(retA1 != nullptr) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (retA1));
    } else {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (retA11));
    }

}

void Riscv::syscall(int code, void* arg1, void* arg2, void* arg3, void(*arg4)(void*)) {

    // kod sistemskog poziva u a0
    __asm__ volatile("mv a0, %[kod]" : : [kod] "r" (code));

    // parametri sistemskog poziva u a1,a2,a3,a4
    if (code == 1) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
    } else if (code == 2) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
    } else if (code == 15) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
    } else if (code == 16) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
        __asm__ volatile("mv a3, %[kod]" : : [kod] "r" (arg2));
        __asm__ volatile("mv a2, %[kod]" : : [kod] "r" (arg4));
        __asm__ volatile("mv a4, %[kod]" : : [kod] "r" (code));
    } else if (code == 17) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
        __asm__ volatile("mv a3, %[kod]" : : [kod] "r" (arg2));
        __asm__ volatile("mv a2, %[kod]" : : [kod] "r" (arg4));
        __asm__ volatile("mv a4, %[kod]" : : [kod] "r" (code));
    } else if (code == 33) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
        __asm__ volatile("mv a2, %[kod]" : : [kod] "r" (arg2));
    } else if (code == 34) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
    } else if (code == 35) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r" (arg1));
    } else if (code == 36) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r"(arg1));
    } else if (code == 66) {
        __asm__ volatile("mv a1, %[kod]" : : [kod] "r"(arg1));
    }

    __asm__ volatile("ecall");
}

void Riscv::popSppSpie() {
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::ConsoleTrap() {
    uint64 scause = r_scause();
    if (scause == 0x8000000000000001UL) {
        mc_sip(SIP_SSIP);
    }
    if (scause == 0x8000000000000009UL) {
        mc_sip(SIP_SSIP);
        //mc_sip(SIP_SEIP);
        console_handler();
    }
}