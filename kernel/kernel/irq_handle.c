#include "x86.h"
#include "device.h"
#include "syscall.h"
#include "schedule.h"
#define NR_IRQ_HANDLE 32

#define NR_HARD_INTR 16
#define VED_ADDR 0xB8000

void k_fork();
extern TSS tss;
extern SegDesc gdt[NR_SEGMENTS];       // the new GDT
extern PCB *current;

struct IRQ_t{
	void (*routine)(void);
	struct IRQ_t *next;
};

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
static struct IRQ_t *handles[NR_HARD_INTR];
static int handle_count = 0;

void
add_irq_handle(int irq, void (*func)(void) ){
    assert(irq < NR_HARD_INTR);
    assert(handle_count <= NR_IRQ_HANDLE);

    struct IRQ_t *ptr;
    ptr = &handle_pool[handle_count++];
    ptr->routine = func;
    ptr->next = handles[irq];
    handles[irq] = ptr;
}

int row;
int column;
char ch;
void scr_write(char c){
	if (c == '\n' || column == 80){
		row++;
		column = 0;
		if (c == '\n') return;
	}
	int *p = (void *)VED_ADDR + (80 * row + column)*2;
	*p = 0x0f00 | c;
	column++;

}
int scr_clr(){
	row = 0; column = 0;
	int i;
	for (i=0; i<25*80; i++){
		scr_write(' ');
	}
	row = 0;
	column = 0;
	return 1;
}
int sys_write(int fd, void *buf, int len) {
	if ((fd==1) || (fd==2)){
		int i;
		for (i=0; i<len; i++){
			ch = ((char*)buf+PBASE(current->no))[i];
			putchar(ch);
			scr_write(ch);
		}
	}
	return len;
}

void do_syscall(struct TrapFrame *tf){
	switch(tf->eax){
		case 0:
			//cli();
			disable_interrupt();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			//sti();
			enable_interrupt();
			break;

		//case SYS_brk:  tf->eax = 0; break;
		case SYS_write:
			tf->eax = sys_write(tf->ebx, (void*)tf->ecx, tf->edx);
			break;
		case SYS_clr:
			tf->eax = scr_clr();
			break;
		case SYS_fork:
			//tf->eax = k_fork();
			k_fork();
			tf->eax = current->regs.eax;
			break;
		default: assert(0);
	}
}

extern PCB *current;
void
irq_handle(struct TrapFrame *tf) {
    /*
     * 中断处理程序
     */
		asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
		asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
    int irq = tf->irq;
    switch(irq) {
			/*
      case 1000:break;
      case 1001:break;

			case 0:		break;
			case 1:		break;
			case 2:		break;
			case 3:		break;
			case 4:		break;
			case 5:		break;
			case 6:		break;
			case 7:		break;
			case 8:		break;
			case 9:		break;
			case 10:	break;
			case 11:	break;
			case 12:	break;
			case 13:	break;
			case 14:	break;
			*/
			case 0x80:
				do_syscall(tf);
				break;
			case 0x20:
				//disable_interrupt();
				putchar('.');
				current->time_count--;
				if (current->time_count == 0){
					schedule();
				}
				//enable_interrupt();
				break;
			case 0x2e:
				break;
   		default:assert(0);
    }
}
