#include "common.h"
#include "x86.h"
#include "device.h"

extern void enter_user_space(void);

void
kentry(void) {
	assert(0);
	init_serial();			//初始化串口输出
	init_idt();
	init_intr();
	init_seg();
	load_umain();
	enter_user_space();
	//while(1);
	assert(0);
}
