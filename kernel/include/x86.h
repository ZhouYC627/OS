#ifndef __X86_H__
#define __X86_H__

#define UADDR 0x200000
#include "x86/cpu.h"
#include "x86/memory.h"
#include "x86/io.h"
#include "x86/irq.h"

void init_seg(void);
uint32_t load_umain(void);
void init_timer(void);
unsigned int getKeyCode();
unsigned int trans_key();
void init_pcb(uint32_t);

#endif
