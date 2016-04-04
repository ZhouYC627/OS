#include "common.h"
#include "x86.h"
#include "device.h"
#include "lib.h"

extern void enter_user_space(uint32_t);

void
kentry(void) {
	init_serial();			//初始化串口输出
	init_idt();
	init_intr();
	init_seg();
	uint32_t entry = load_umain();
	assert(entry==0);
	assert(0);
	enter_user_space(entry);
	while(1);
	assert(0);
}
