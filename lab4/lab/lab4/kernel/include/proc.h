#ifndef __PROC_H__
#define __PROC_H__

#include "process/pcb.h"
#include "process/schedule.h"
#include "process/sem.h"

#define SYS_fork 20
#define SYS_sleep 21
#define SYS_exit 22
#define SYS_createSem 23
#define SYS_unlockSem 24
#define SYS_lockSem 25
#define SYS_destroySem 26
#define time_chips 10

#endif
