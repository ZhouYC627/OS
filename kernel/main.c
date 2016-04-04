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
	print_d((double)entry,15);
	enter_user_space(entry);
	while(1);
}
