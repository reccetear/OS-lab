#include "common.h"
#include "x86.h"
#include "device.h"
#include "proc.h"

void
kentry(void) {
	init_serial();			//初始化串口输出
    	init_idt();
	init_intr();
    	init_seg();
	init_timer();
	init_idle();
	create_uthread(load_umain());
	enable_interrupt();
    	while(1){
		putchar('^');
		wait_for_interrupt();
	}
	assert(0);
}
