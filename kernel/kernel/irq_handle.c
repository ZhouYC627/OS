#include "x86.h"
#include "device.h"

void
irq_handle(struct TrapFrame *tf) {
    /*
     * 中断处理程序
     */
    switch(tf->irq) {
        case 1000:break;
        case 1001:break;
        default:assert(0);
    }
}

