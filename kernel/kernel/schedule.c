#include "x86.h"
#include "device.h"
#include "schedule.h"
//#include "../../lib/string.h"


PCB pcb_table[MAX_PCB];
PCB idle, *current = &idle;
PCB *block;
unsigned int PID;

extern TSS tss;
extern SegDesc gdt[NR_SEGMENTS];       // the new GDT

void init_pcb(uint32_t entry){
  int i;
  for (i=0; i<MAX_PCB; i++){
    pcb_table[i].state = FREE;
    pcb_table[i].no = i;
    //pcb_table[i].next = NULL;
  }
  block = NULL;
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
  pcb_table[P1].regs.esp = 0x200000;
  pcb_table[P1].regs.eflags = 0x246;
  pcb_table[P1].regs.cs = USEL(SEG_UCODE);
  pcb_table[P1].regs.eip = entry;

  //return idle.regs.esp;

}
void memcpy(unsigned char* dst, unsigned char* src, uint32_t size){
  int i;
  for (i=0; i<=size; i++){
    *(dst+i) = *(src+i);
  }
}

void schedule(void){
  if (current == current->next){      //only one process in queue
    current->time_count = SLICESIZE;
    putchar('R');
    return;
  }
  putchar('S');
  putchar(current->next->pid +'0');
  current->state = READY;
  current->time_count = SLICESIZE;
  current = current->next;
  tss.esp0 = (uint32_t)&current->regs + sizeof(stackframe);
  current->state = RUNNING;
  current->time_count = SLICESIZE;

  gdt[SEG_UCODE] = SEG(STA_X | STA_R, PBASE(current->no), 0xffffffff, DPL_USER);
  gdt[SEG_UDATA] = SEG(STA_W,         PBASE(current->no), 0xffffffff, DPL_USER);
}

void k_fork(){
  PCB *child = current;

  int i = 0;        //find a free pcb
  for (; i<MAX_PCB; i++){
    if (pcb_table[i].state == FREE){
      child = (PCB*)&pcb_table[i];
      putchar(i+'0');
      break;
    }
  }
  child->regs = current->regs;
  child->state = READY;
  child->time_count = SLICESIZE;
  child->pid = PID ++;
  memcpy(child->k_stack, current->k_stack, STACKSIZE);
  memcpy((unsigned char*)PBASE(child->no), (unsigned char*)PBASE(current->no), 0x200000);
  child->next = current->next;
  current->next = child;
  //current->time_count = 1;
  child->regs.eax = 0;
  current->regs.eax = child->pid;
  //return child->pid;
}

void k_sleep(int t){
  current->state = BLOCKED;
  current->sleep_time = t * 1000;
  //current->time_count = 1;
  PCB *p = current;
  while (p->next != current){
    p = p->next;
  }
  p->next = current->next;
  current->next = block;
  block = current;
  current = p;
  if (current == block){  //no runnable process
    current = &idle;
    current->next = current;
  }
  schedule();

}
void check_sleep(){
  PCB *p = block;
  while (p!=NULL){
    p->sleep_time--;
    if (p->sleep_time == 0){
      p->state = READY;
      p->time_count = SLICESIZE;
      PCB *q = block;
      if (p == block){
        block = p->next;
      }else{
        while(q->next != p){
          q = q->next;
        }
        q->next = p->next;
      }
      p->next = current->next;
      current->next = p;
      p = q;
    }
    if (p == NULL) break;
    p = p->next;
  }
}
