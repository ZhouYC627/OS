#include "common.h"
#include "x86.h"
#include "device.h"

extern void enter_user_space(uint32_t);

void
kentry(void) {
	init_serial();			//初始化串口输出
	init_idt();
	init_intr();
	init_seg();
	uint32_t entry = load_umain();
	enter_user_space(entry);
	//assert(0);
	while(1);
	assert(0);
}
