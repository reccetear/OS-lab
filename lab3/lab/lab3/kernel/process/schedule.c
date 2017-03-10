#include "proc.h"

void schedule(void) {
	if(!(list_empty(&readyq_h)) && (current->state == READY))
	{
		struct list_head *tmp;
		tmp = current->state_list.next;
		if(tmp == &readyq_h)
		{
			tmp = tmp->next;
		}
		//search next pcb
		int i;
		for(i = 0; PCBs[i].state_list.next != tmp->next && PCBs[i].state_list.prev != tmp->prev; i++);
		PCB *next = (void *)&(PCBs[i]);
		//fresh new pcb time count
		if(current->pid != 0)
		{
			current->time_count = time_chips;
		}
		//save current pcb
		struct p_process_table *frame = current->sf;
		current->sf = (struct p_process_table *)(current->p_stack + STACK_SIZE) - 1;
		current->sf->p_trap.ds = frame->p_trap.ds;
		current->sf->p_trap.es = frame->p_trap.es;
		current->sf->p_trap.eax = frame->p_trap.eax;
		current->sf->p_trap.ebx = frame->p_trap.ebx;
		current->sf->p_trap.ecx = frame->p_trap.ecx;
		current->sf->p_trap.edx = frame->p_trap.edx;
		current->sf->p_trap.esi = frame->p_trap.esi;
		current->sf->p_trap.edi = frame->p_trap.edi;
		current->sf->p_trap.xxx = frame->p_trap.xxx;
		current->sf->p_trap.irq = frame->p_trap.irq;
		current->sf->ss = frame->ss;
		current->sf->esp = frame->esp;
		current->sf->eip = frame->eip;
		current->sf->eflags = frame->eflags;
		current->sf->cs = frame->cs;
		//change current pcb
		current = next;
	}
	else
	{
		current = &PCBs[0];
	}
}

void change_state(void) {
	if(current->state == SLEEP && current->sleep_time > 0)
	{
		current->sleep_time--;
		assert(current->sleep_time >= 0);	//sleep time should not less than 0
		//sleep continue
		sys_sleep(current->sleep_time);
	}
	else if(current->state == SLEEP && current->sleep_time == 0)
	{
		assert(current->sleep_time == 0);
		//it's time to wake up
		wake_up();
	}
	else if(current->state == READY)
		;	//no change
	else
		assert(0);		//should not reach here
}
