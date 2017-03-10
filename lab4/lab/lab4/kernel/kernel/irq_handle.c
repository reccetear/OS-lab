#include "x86.h"
#include "device.h"
#include "proc.h"
#define SYS_write 4

union character {
	uint16_t val;
	struct {
	uint8_t ch;
	uint8_t color;
	};
};

void
do_syscall(struct TrapFrame *tf);

int
write(int fd, char *buf, int len, int line, int row) {
	if(fd == 1 || fd == 2)
	{
		uint16_t *gs = (void *)0xb8000;
		gs = gs + line * 80 + row;
		union character c;
		int i;
		for(i = 0; i < len; i++)
		{
			c.ch = (buf+0x200000)[i];
			c.color = 0x0b;
			gs[i] = c.val;
		}
		return len;
	}
	assert(0);
	return -1;
}

void
irq_handle(struct TrapFrame *tf) {
    /*
     * 中断处理程序
     */
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	if(current->state == READY)
		current->sf = (void *)tf;		//save old trapframe
    	switch(tf->irq) {
		case 0x80:do_syscall(tf);break;
		case 13:assert(0);break;
		case 1000:
		{
			change_state();
			if(current->state == READY && current->time_count > 0)
			{
				current->time_count--;
				assert(current->time_count >= 0);	//time count should not less than 0
			}
			else
			{
				assert(current->time_count == 0);
				assert(current->state == READY);
				schedule();
				if(current->pid != 0)
					current->time_count--; 		//schedule has run once , should decrease one on time count
			}
		}break;
		case 1014:break;
        	default:assert(0);
    	}
}

void
do_syscall(struct TrapFrame *tf) {
	switch(tf->eax)
	{
		case SYS_write:tf->eax = write(tf->edx,(void *)tf->ecx,tf->ebx,tf->edi,tf->esi);break;
		case SYS_fork:tf->eax = sys_fork();break;
		case SYS_sleep:sys_sleep(tf->edx);break;
		case SYS_exit:sys_exit(tf->edx);break;
		case SYS_createSem:create_sem();break;
		case SYS_unlockSem:unlocksem();break;
		case SYS_lockSem:locksem();break;
		case SYS_destroySem:destroysem();break;
		default: assert(0);
	}
}
