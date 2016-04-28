#include "x86.h"
#include "device.h"
#include "schedule.h"
//#include "../../lib/string.h"


PCB pcb_table[MAX_PCB];
PCB idle, *current = &idle;
unsigned int PID;

extern TSS tss;

void init_pcb(uint32_t entry){
  int i;
  for (i=0; i<MAX_PCB; i++){
    pcb_table[i].state = FREE;
  }
  PID = 0;
  idle.pid = PID ++;
  idle.state = RUNNING;
  idle.time_count = SLICESIZE;
  idle.next = (PCB*)&pcb_table[P1];

  //first process
  pcb_table[P1].state = READY;
  pcb_table[P1].time_count = SLICESIZE;
  pcb_table[P1].pid = PID ++;
  pcb_table[P1].next = 	(PCB*)&pcb_table[P1];
  //pcb_table[P1].regs.eip = entry;
  pcb_table[P1].regs.ds = USEL(SEG_UDATA);
  pcb_table[P1].regs.es = USEL(SEG_UDATA);
  pcb_table[P1].regs.ss = USEL(SEG_UDATA);
  pcb_table[P1].regs.esp = 0x400000;
  pcb_table[P1].regs.eflags = 0x246;
  pcb_table[P1].regs.cs = USEL(SEG_UCODE);
  pcb_table[P1].regs.eip = entry;

  //return idle.regs.esp;

}
void memcpy(unsigned char* dst, unsigned char* src, uint32_t size){
  int i;
  for (i=0; i<size; i++){
    *(dst+i) = *(src+i);
  }
}

void schedule(void){
  putchar('S');
  if (current == current->next){      //only one process in queue
    current->time_count = SLICESIZE;
    putchar('R');
    return;
  }
  current->state = READY;
  current->time_count = SLICESIZE;
  current = current->next;
  tss.esp0 = (uint32_t)&current->regs + sizeof(stackframe);
  current->state = RUNNING;
  current->time_count = SLICESIZE;
}

uint32_t k_fork(){
  PCB *child = current;

  int i = 0;        //find a free pcb
  for (; i<MAX_PCB; i++){
    if (pcb_table[i].state == FREE){
      child = (PCB*)&pcb_table[i];
      break;
    }
  }
  child->regs = current->regs;
  child->state = READY;
  child->time_count = SLICESIZE;
  child->pid = PID++;
  memcpy(child->k_stack, current->k_stack, STACKSIZE);
  child->next = current->next;
  current->next = child;
  return child->pid;

}
