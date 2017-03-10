#include "lib.h"

void do_child() {
	int i;
	for(i = 0; i < 10; i++)
	{
		locksem();
		printf("pang\n");
	}
}

void do_father() {
	int i;
	for(i = 0; i < 10; i++)
	{
		printf("ping\n");
		unlocksem();
		sleep(1);
	}	
}

void
uentry(void){
	createSem();
	int fpid;
	fpid = fork();
	if(fpid < 0)
		printf("error in fork\n");
	if(fpid == 0)
		do_child();
	else
		do_father();
	printf("main end , return to idle\n");
	destroySem();
	exit(0);
}
