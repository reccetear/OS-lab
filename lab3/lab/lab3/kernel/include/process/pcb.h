#ifndef __PCB_H__
#define __PCB_H__

#include "x86.h"
#include "common.h"
#include "device.h"

#define NR_PCBS 512
#define SLEEP 1
#define READY 0
#define FREE 2
#define STACK_SIZE 4096

/*PCB struct */
typedef struct p_task_table {
	struct p_process_table *sf;		//process stack frame
	uint32_t 	pid;				//process id
	volatile long 	state;			//process state:run wait ready
	int time_count;				//time
	int sleep_time;				//sleep time
	int lock_depth;
	list_head 	state_list;		//process state list
	char name[32];
	uint8_t p_stack[STACK_SIZE];
} PCB;

extern PCB 	PCBs[NR_PCBS];			//total PCBs
extern PCB	*current;			//current pcb
extern list_head	readyq_h,blockq_h;	//ready queue;block queue
extern uint16_t		pcbs_avl;		//PCB available	index

void init_idle(void);
PCB *create_uthread(uint32_t entry);
int sys_fork();
void sys_sleep(uint32_t time);
void wake_up();
void sys_exit(int status);

#endif
