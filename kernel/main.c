#include "common.h"
#include "x86.h"
#include "device.h"
#include "lib.h"

extern void enter_user_space(uint32_t);

void p_idle(){
	while(1){
		wait_for_interrupt();
	}
}

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
	p_idle();
	//enter_user_space(entry);
	while(1);
}
