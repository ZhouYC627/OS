#include "common.h"
#include "x86.h"
#include "device.h"
#include "lib.h"
#include "schedule.h"

extern void enter_user_space(uint32_t);
extern PCB idle;

//void add_irq_handle(int, void (*)(void));


void
kentry(void) {
	init_serial();			//初始化串口输出
	init_idt();
	init_timer();
	init_intr();
	init_seg();

	uint32_t entry = load_umain();
	init_pcb(entry);
	//putchar('A');
	asm volatile("movl %%eax, %%esp" ::"a"(&idle.regs.esp));
	enable_interrupt();
	while(1){
		//putchar('^');
		wait_for_interrupt();
	}

	///p_idle();
	//enter_user_space(entry);
	while(1);
}
