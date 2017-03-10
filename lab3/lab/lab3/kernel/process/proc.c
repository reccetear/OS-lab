#include "proc.h"
#include "common.h"
#include "x86.h"
#include "device.h"

#define USER_CS_BASE 0x200000
#define USER_SS_BASE 0x200000

PCB PCBs[NR_PCBS];
PCB *current;
LIST_HEAD(readyq_h);
LIST_HEAD(freeq_h);
LIST_HEAD(blockq_h);
uint16_t pcbs_avl = 0;		//PCBs available index

void init_idle() {
	PCB *pcb = &(PCBs[0]);
	current = pcb;
	//don't need to set registers(for no iret)
	current->pid = 0;
	current->state = READY;
	current->time_count = 0;
	current->sleep_time = 0;
	current->lock_depth = 0;
	//add to ready queue
	list_add(&(current->state_list),&readyq_h);
	pcbs_avl = 1;
	char *str = "idle";
	int i;
	for(i = 0; i < 5; i++)
		current->name[i] = str[i];
}

PCB *create_uthread(uint32_t entry) {
	//allocate a pcb
	assert(pcbs_avl < NR_PCBS);
	PCB *pcb = &(PCBs[pcbs_avl]);
	pcb->pid = pcbs_avl;
	pcb->lock_depth = 0;

	//set space to restore trapframe
	pcb->sf = (struct p_process_table *)(pcb->p_stack + STACK_SIZE) - 1;
	//set initial registers
	pcb->sf->p_trap.ds= USEL(SEG_UDATA);
	pcb->sf->p_trap.es= USEL(SEG_UDATA);
	pcb->sf->p_trap.eax = 0;
	pcb->sf->p_trap.ebx = 0;
	pcb->sf->p_trap.ecx = 0;
	pcb->sf->p_trap.edx = 0;
	pcb->sf->p_trap.esi = 0;
	pcb->sf->p_trap.edi = 0;
	pcb->sf->p_trap.xxx = 0;
	pcb->sf->p_trap.irq = 0;

	pcb->sf->ss = USEL(SEG_UDATA);
	pcb->sf->eflags = 0x202;
	pcb->sf->esp = 0xc00000;
	pcb->sf->cs = USEL(SEG_UCODE);
	pcb->sf->eip = (uint32_t)entry;

	pcb->time_count = time_chips;
	pcb->sleep_time = 0;

	pcb->state = READY;
	char *str = "PCB";
	int i;
	for(i = 0; i < 3; i++)
		pcb->name[i] = str[i];
	pcb->name[i] = '0' + pcbs_avl;
	pcbs_avl++;
	pcb->name[i + 1] = '\0';
	list_add(&(pcb->state_list),&readyq_h);
	return pcb;
}

int sys_fork() {
	//allocate a pcb
	assert(pcbs_avl < NR_PCBS);
	PCB *pcb = &(PCBs[pcbs_avl]);
	pcb->pid = pcbs_avl;
	pcb->lock_depth = 0;

	pcb->sf = (struct p_process_table *)(pcb->p_stack + STACK_SIZE) - 1;
	//copy father to child
	//general registers are copied to new pcb;
	pcb->sf->p_trap.ds = current->sf->p_trap.ds;
	pcb->sf->p_trap.es = current->sf->p_trap.es;
	pcb->sf->p_trap.eax = pcb->pid;
	pcb->sf->p_trap.ebx = current->sf->p_trap.ebx;
	pcb->sf->p_trap.ecx = current->sf->p_trap.ecx;
	pcb->sf->p_trap.edx = current->sf->p_trap.edx;
	pcb->sf->p_trap.esi = current->sf->p_trap.esi;
	pcb->sf->p_trap.edi = current->sf->p_trap.edi;
	pcb->sf->p_trap.xxx = current->sf->p_trap.xxx;
	pcb->sf->p_trap.irq = current->sf->p_trap.irq;

	pcb->sf->ss = current->sf->ss;
	pcb->sf->eflags = current->sf->eflags;
	pcb->sf->cs = current->sf->cs;
	pcb->sf->eip = current->sf->eip;

	//copy stack(assert user whole stack size less than 4096)
	pcb->sf->esp = current->sf->esp - STACK_SIZE;
	uint8_t *father = (void *)(current->sf->esp + USER_SS_BASE);
	uint8_t *son = (void *)(pcb->sf->esp + USER_SS_BASE);
	int i;
	for(i = 0; i < STACK_SIZE; i++)
		son[i] = father[i];

	pcb->time_count = time_chips;
	pcb->sleep_time = 0;

	pcb->state = READY;
	char *str = "PCB";
	for(i = 0; i < 3; i++)
		pcb->name[i] = str[i];
	pcb->name[i] = '0' + pcbs_avl;
	pcbs_avl++;
	pcb->name[i + 1] = '\0';
	list_add(&(pcb->state_list),&readyq_h);
	return 0;
}

void lock() {
	current->lock_depth++;
	disable_interrupt();
}

void unlock() {
	current->lock_depth--;
	if (current->lock_depth == 0) {
		enable_interrupt();
	}
}

void sys_sleep(uint32_t time) {
	lock();
	if (current->state == READY) {
		current->state = SLEEP;
		current->sleep_time = time * 100;
		list_del(&(current->state_list));
		list_add(&(current->state_list),&blockq_h);
	}
	unlock();
	while(1)
		wait_for_interrupt();
}

void wake_up() {
	lock();
	if(current->state == SLEEP) {
		current->state = READY;
		list_del(&(current->state_list));
		list_add(&(current->state_list),&readyq_h);
	}
	unlock();
}

void sys_exit(int status) {
	lock();
	if(current->state == READY) {
		//assert(current->time_count != 0);
		PCB *free_pcb = current;

		struct list_head *tmp;
		tmp = current->state_list.next;
		//search next pcb
		int i;
		for(i = 0; PCBs[i].state_list.next != tmp->next && PCBs[i].state_list.prev != tmp->prev; i++);
		current = (void *)&(PCBs[i]);

		free_pcb->state = FREE;
		list_del(&(free_pcb->state_list));
		list_add(&(free_pcb->state_list),&freeq_h);
	}
	unlock();
}
