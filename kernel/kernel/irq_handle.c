#include "x86.h"
#include "device.h"
#define NR_IRQ_HANDLE 32

#define NR_HARD_INTR 16

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

char ch;
int sys_write(int fd, void *buf, int len) {
	if ((fd==1) || (fd==2)){
		int i;
		for (i=0; i<len; i++){
			ch = ((char*)buf+UADDR)[i];
			putchar(ch);
		}
	}
	return len;
}

void do_syscall(struct TrapFrame *tf){
		assert(0);
	switch(tf->eax){
		case 0:
			//cli();
			disable_interrupt();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			//sti();
			enable_interrupt();
			break;
		
		//case SYS_brk:  tf->eax = 0; break;
		case 4:
			tf->eax = sys_write(tf->ebx, (void*)tf->ecx, tf->edx);
			break;
		default: assert(0);
	}
}

void 
irq_handle(struct TrapFrame *tf) {
    /*
     * 中断处理程序
     */
    int irq = tf->irq;
    switch(irq) {
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
	case 0x80:
		do_syscall(tf);
        default:assert(0);
    }
}

