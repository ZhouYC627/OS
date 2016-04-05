#include "lib.h"
#include "types.h"
/*
 * io lib here
 * 库函数写在这
 */

static inline int32_t syscall(int num, int check, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
//static inline int32_t syscall(int num, ...)
{
	int32_t ret = 0;
	//Generic system call: pass system call number in AX
	//up to five parameters in DX,CX,BX,DI,SI
	//Interrupt kernel with T_SYSCALL
	//
	//The "volatile" tells the assembler not to optimize
	//this instruction away just because we don't use the
	//return value
	//
	//The last clause tells the assembler that this can potentially
	//change the condition and arbitrary memory locations.

	/*Lab2 code here
	  嵌入汇编代码，调用int $0x80
	 */
	//int *args = &num;
	asm volatile("int 0x80" : "=a"(ret) : "a"(a1), "b"(a2), "c"(a3), "d"(a4));
	return ret;
}
/*
void printc(char *c){
	syscall(4, 1, c, 1);
}
*/

void printf(const char *format,...){
	//syscall(4, );
	/*
	static char buf[256];
	void *args = (void**)&format + 1;
	vsnprintf(buf, 256, format, args);
	*/
} 

