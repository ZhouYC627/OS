#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#define MAX_PCB 20
#define STACKSIZE 256
#define SLICESIZE 10
#define P1 0

typedef struct p_process_table{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, xxx, ebx, edx, ecx, eax;
    int32_t irq, ecode;
    uint32_t eip, cs, eflags, esp, ss;
}stackframe;

typedef struct p_task_table{
    unsigned int k_stack[STACKSIZE];
    stackframe regs;
    int state;
    int time_count;
    int sleep_time;
    unsigned int pid;
    //char name[32];
    struct p_task_table* next;
}PCB;

enum {READY, RUNNING, BLOCKED, DEAD, EMPTY};
void schedule();

#endif
