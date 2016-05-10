#include "x86.h"
#include "device.h"
#include "schedule.h"

semaphore semp;
extern PCB *current;
int ss = 0;
void createSem(semaphore *s){
  //semp.value = 0;
  ss = 0;
  //semp.list = (PCB*)&current;
  s = &semp;
  s->list = current;
  s->value = &ss;
}

void lockSem(semaphore *s){
    //*s->value -=1;
    ss --;
    if(ss < 0){
      s->list = current;
      current->sleep_time = -1;
      current->state = BLOCKED;
      schedule();
    }
}
void unlockSem(semaphore *s){
    //*s->value +=1;
    ss++;
    if(ss >= 0){
      PCB *p = s->list;
      //p->state = READY;
      //p->time_count = SLICESIZE;
      p->sleep_time = 1;
      current->time_count = 1;
      //schedule();
    }
        //R(s->list);
}

void destorySem(semaphore *s){
  s->value = 0;
  s->list = NULL;
}
