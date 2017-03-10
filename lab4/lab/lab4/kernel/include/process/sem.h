#ifndef __SEM_H__
#define __SEM_H__

typedef struct Semaphore {
	int value;
	list_head queue;
} Semaphore;

void init_sem(Semaphore *sem,int value);
void create_sem(void);
void V(Semaphore *sem);
void P(Semaphore *sem);
void unlocksem(void);
void locksem(void);
void destroysem(void);

#endif
