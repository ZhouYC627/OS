#include "x86.h"
#include "device.h"
#include "schedule.h"
//#include "../../lib/string.h"


PCB pcb_table[MAX_PCB];
PCB idle, *current = &idle;
//int num_of_ready = 0;
//PCB *block = &idle;
unsigned int PID;

extern TSS tss;
extern SegDesc gdt[NR_SEGMENTS];       // the new GDT

void init_pcb(uint32_t entry){
  int i;
  for (i=0; i<MAX_PCB; i++){
    pcb_table[i].state = FREE;
    pcb_table[i].no = i;
    //pcb_table[i].next = 0;
  }
  PID = 0;

  //block->state = BLOCKED;
  //block->next = block;
  //idle.no = -1;
  idle.pid = PID++;
  idle.state = RUNNING;
  idle.time_count = SLICESIZE;
  idle.next = (PCB*)&pcb_table[P1];
  idle.no = -1;

  //first process
  pcb_table[P1].state = READY;
  pcb_table[P1].time_count = SLICESIZE;
  pcb_table[P1].pid = PID++;
  pcb_table[P1].next = 	(PCB*)&pcb_table[P1];
  //pcb_table[P1].regs.eip = entry;
  pcb_table[P1].regs.ds = USEL(SEG_UDATA);
  pcb_table[P1].regs.es = USEL(SEG_UDATA);
  pcb_table[P1].regs.ss = USEL(SEG_UDATA);
  pcb_table[P1].regs.esp = 0x200000;
  pcb_table[P1].regs.eflags = 0x2;
  pcb_table[P1].regs.cs = USEL(SEG_UCODE);
  pcb_table[P1].regs.eip = entry;
  //return idle.regs.esp;
}

void k_memcpy(unsigned char* dst, unsigned char* src, uint32_t size){
  int i;
  for (i=0; i<size; i++){
    *(dst+i) = *(src+i);
  }
}
/*
void k_memset(unsigned char* dst, char c, uint32_t size){
  int i;
  for (i=0; i<=size; i++){
    *(dst+i) = c;
  }
}
*/
void schedule(void){
  /*if (current == current->next && current->state == RUNNING){      //only one process in queue
    current->time_count = SLICESIZE;
    putchar('R');
    return;
  }*/
  putchar('S');
  if (current->state == RUNNING){
    current->state = READY;
    current->time_count = SLICESIZE;
  }
  PCB *p = current->next;
  while(p->state != READY){
      if (p == current){
        p = &idle;
        p->next = p;
      }
      p = p->next;
  }
  current = p;
  putchar(current->pid +'0');
  /*if (num_of_ready == 0){
    current = &idle;
    current->next = current;
  }else{
      while (current->state != READY){
        current = current->next;
      }
  }*/
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
  //num_of_ready++;
  child->regs = current->regs;
  child->state = READY;
  child->time_count = SLICESIZE;
  child->pid = PID ++;
  k_memcpy(child->k_stack, current->k_stack, STACKSIZE);
  k_memcpy((unsigned char*)PBASE(child->no), (unsigned char*)PBASE(current->no), 0x200000);
  child->next = current->next;
  current->next = child;
  //current->time_count = 1;
  child->regs.eax = 0;
  current->regs.eax = child->pid;
  //return child->pid;
}

void k_sleep(int t){
  current->state = BLOCKED;
  current->sleep_time = t/10;

  schedule();

}

void check_sleep(){
  //PCB *p = current;
  int i;
  //int count =0;
  //if (current == &idle) return;
  for(i=0; i< MAX_PCB; i++){
    //if (p == p->next && p->state ==READY) break;
    //if (count++ > num_of_ready) break;
    PCB *p = (PCB*)&pcb_table[i];
    if (p->state == BLOCKED && p->sleep_time==0){
      putchar('R');
      putchar(p->pid + '0');
      p->state = READY;
      p->time_count = SLICESIZE;
      if (current == &idle){
        current->next = p;
      }
      //num_of_ready++;
      }
    if (p->state == BLOCKED){
      p->sleep_time--;
    }
  }
}

int k_exit(){
  int re = 0;
  current->state = FREE;
  //k_memset(current->k_stack, 0, STACKSIZE);
  //k_memset((void*)PBASE(current->no), 0, 0x200000);
  PCB *p = current;
  while (p->next != current){
    p = p->next;
  }
  p->next = current->next;
  if (current == current->next){ //idle
    current = &idle;
    current->next = current;
    re = 0;
  }else{
    current = p;
    re = 1;
  }
  schedule();
  //putchar('e');
  return re;
}
