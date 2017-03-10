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
		PCB *next = (void *)((char *)tmp - 0x3c);
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
		current->sf->p_trap.ebp = frame->p_trap.ebp;
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
	else if(current->state == SLEEP)
	{
		//save current pcb
		struct p_process_table *frame = current->sf;
		current->sf = (struct p_process_table *)(current->p_stack + STACK_SIZE) - 1;
		current->sf->p_trap.ds = frame->p_trap.ds;
		current->sf->p_trap.es = frame->p_trap.es;
		current->sf->p_trap.eax = frame->p_trap.eax;
		current->sf->p_trap.ebx = frame->p_trap.ebx;
		current->sf->p_trap.ecx = frame->p_trap.ecx;
		current->sf->p_trap.edx = frame->p_trap.edx;
		current->sf->p_trap.ebp = frame->p_trap.ebp;
		current->sf->p_trap.esi = frame->p_trap.esi;
		current->sf->p_trap.edi = frame->p_trap.edi;
		current->sf->p_trap.xxx = frame->p_trap.xxx;
		current->sf->p_trap.irq = frame->p_trap.irq;
		current->sf->ss = frame->ss;
		current->sf->esp = frame->esp;
		current->sf->eip = frame->eip;
		current->sf->eflags = frame->eflags;
		current->sf->cs = frame->cs;

		//find next pcb
		struct list_head *tmp;
		tmp = readyq_h.next;
		if(tmp == &readyq_h)
			assert(0);
		else
		{
			current = (void *)((char *)tmp - 0x3c);
			assert(current->state == READY);
		}
	}
	else if(current->state == FREE)
	{
		//find next pcb
		struct list_head *tmp;
		tmp = readyq_h.next;
		if(tmp == &readyq_h)
			assert(0);
		else
		{
			current = (void *)((char *)tmp - 0x3c);
			assert(current->state == READY);
		}
	}
	else
		current = &PCBs[0];
}

void change_state(void) {
	if(!(list_empty(&blockq_h)))		//blockq are not empty
	{
		//find all block pcbs
		struct list_head *tmp;
		for(tmp = blockq_h.next; tmp != &blockq_h;)
		{
			//search next pcb
			PCB *block = (void *)((char *)tmp - 0x3c);
			assert(block->state == SLEEP);

			if(block->sleep_time > 0)
			{
				//sleep continue
				block->sleep_time--;
				tmp = tmp->next;
			}
			else if(block->sleep_time == -1)
			{
				//need V
				tmp = tmp->next;
			}
			else
			{
				tmp = block->state_list.next;
				assert(block->sleep_time == 0);
				//it's time to wake up
				wake_up(block);
			}
		}
	}
}
