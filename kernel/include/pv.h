#ifndef __PV_H_
#define __PV_H_
#include "schedule.h"
typedef struct s_semaphore{
    int *value;
    struct p_task_table *list;
    int v;
}semaphore;

void createSem(semaphore*);
void lockSem(semaphore*);
void unlockSem(semaphore*);

#endif
