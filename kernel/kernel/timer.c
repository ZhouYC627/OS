#include "x86.h"
//#include "common.h"

#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

volatile uint32_t time_tick = 0;


void init_timer(void) {
    time_tick = 0;
    int counter = FREQ_8253 / HZ;
    //assert(TIMER_PORT < 65536);
    out_byte(TIMER_PORT + 3, 0x34);
    out_byte(TIMER_PORT + 0, counter % 256);
    out_byte(TIMER_PORT + 0, counter / 256);
}
