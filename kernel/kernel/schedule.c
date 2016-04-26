#include "x86.h"
#include "schedule.h"


PCB pcb_table[MAX_PCB];
PCB idle, *current = &idle;
unsigned int PID;

void init_pcb(uint32_t entry){
  PID = 0;
  idle.pid = PID ++;
  idle.state = RUNNING;
  idle.time_count = SLICESIZE;
  idle.next = (PCB*)(pcb_table + sizeof(PCB)*P1);

  //first process
  pcb_table[P1].state = READY;
  pcb_table[P1].time_count = SLICESIZE;
  pcb_table[P1].pid = PID++;
  pcb_table[P1].next = 	(PCB*)(pcb_table + sizeof(PCB)*P1);
  pcb_table[P1].regs.eip = entry;
  pcb_table[P1].regs.ds = USEL(SEG_UDATA);
  pcb_table[P1].regs.ss = USEL(SEG_UDATA);
  pcb_table[P1].regs.esp = 0x400000;
  pcb_table[P1].regs.eflags = 0x2;
  pcb_table[P1].regs.cs = USEL(SEG_UCODE);
  pcb_table[P1].regs.eip = entry;




}

void schedule(void){
  if (current == current->next){      //only one process in queue
    current->counter = SLICESIZE;
    return;
  }
  current->state = READY;
  current = current->next;
  tss.esp0 = (uint32_t*)current->regs + sizeof(stackframe);
  current->state = RUNNING;
}
