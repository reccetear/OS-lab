#include "proc.h"

void init_sem(Semaphore *sem,int value) {
	sem->value = value;
	INIT_LIST_HEAD(&sem->queue);
}

void create_sem(void) {
	current->sem = (void *)(current->p_stack);
	init_sem(current->sem,0);
}

void
P(Semaphore *sem) {
	lock();
	sem->value--;
	if (sem->value < 0) {
		list_del(&(current->state_list));
		list_add_tail(&(current->state_list), &(sem->queue));
		sleep();
	}
	unlock();
}

void
V(Semaphore *sem) {
	lock();
	sem->value ++;
	if (sem->value <= 0) {
		assert(!list_empty(&sem->queue));
		PCB *p = (void *)((char *)(sem->queue.next) - 0x3c);
		wake_up(p);
	}
	unlock();
}

void unlocksem() {
	V(current->sem);
}

void locksem() {
	P(current->sem);
}

void destroysem() {
	current->sem->value = 0;
	current->sem->queue.prev = NULL;
	current->sem->queue.next = NULL;
}
