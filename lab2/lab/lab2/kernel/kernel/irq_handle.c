#include "x86.h"
#include "device.h"
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
			c.ch = (buf + 0x200000)[i];
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
    	switch(tf->irq) {
		case 0x80: do_syscall(tf);break;
		case 3:write(2,"HIT BREAKPOINT EXCEPTION(13)",28,8,0);while(1);break;
		case 13:write(2,"HIT PROTECTION EXCEPTION(13)",28,8,0);while(1);break;
		case 1000:assert(0);break;
		case 1001:assert(0);break;
		case 1014:assert(0);break;
        	default:assert(0);
    	}
	asm volatile("movw %%ax,%%es":: "a" (USEL(SEG_UDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (USEL(SEG_UDATA)));
}

void
do_syscall(struct TrapFrame *tf) {
	switch(tf->eax)
	{
		case SYS_write:tf->eax = write(tf->edx,(void *)tf->ecx,tf->ebx,tf->edi,tf->esi);break;
		default: assert(0);
	}
}

