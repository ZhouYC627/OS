#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "pv.h"

#define MAX_PCB 10
#define STACKSIZE 512
#define SLICESIZE 10
#define P1 0

#define PBASE(i) ((i+1) * 0x200000)
#define USEL(desc) (((desc) << 3) | DPL_USER)

typedef struct p_process_table{
    uint32_t es, ds;
    uint32_t edi, esi, ebp, xxx, ebx, edx, ecx, eax;
    int32_t irq;
    uint32_t eip, cs, eflags, esp, ss;
}stackframe;

typedef struct p_task_table{
    unsigned char k_stack[STACKSIZE];
    stackframe regs;
    int state;
    int time_count;
    int sleep_time;
    unsigned int pid;
    semaphore *sem;
    int no;
    //char name[32];
    struct p_task_table* next;
}PCB;

enum {READY, RUNNING, BLOCKED, FREE};
void schedule();
void check_sleep();
void k_fork();
int k_exit();
void k_sleep(int);

#endif
