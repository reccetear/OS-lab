#ifndef __TIMER_H__
#define __TIMER_H__

#include "common.h"
#include "x86.h"

#define HZ 100
#define TIMER_PORT 0x40
#define FREQ_8253 1193182

void init_timer(void);
#endif
