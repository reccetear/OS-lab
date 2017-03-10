#include "device/timer.h"

void
init_timer(void) {
	int counter = FREQ_8253 / HZ;
	assert(counter < 65536);
	out_byte(TIMER_PORT + 3, 0x34);
	out_byte(TIMER_PORT + 0, counter % 256);
	out_byte(TIMER_PORT + 0, counter / 256);
}
