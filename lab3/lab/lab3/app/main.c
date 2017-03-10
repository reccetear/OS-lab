#include "lib.h"

void do_child() {
	int i;
	for(i = 0; i < 10; i++)
	{
		printf("Pang ");
		printf("now sleep for %ds\n",1);
		sleep(1);
	}
}

void do_father() {
	int i;
	for(i = 0; i < 10; i++)
	{
		printf("Ping ");
		printf("now sleep for %ds\n",1);
		sleep(1);
	}
}

void
uentry(void){
	printf("enter uentry\n");
	unsigned int fpid;
	fpid = fork();
	if(fpid < 0)
		printf("error in fork\n");
	if(fpid == 0)
	{
		printf("father = %d\n",fpid);
		do_father();
	}
	else
	{
		printf("child = %d\n",fpid);
		do_child();
	}
	printf("main end , return to idle\n");
	exit(0);
}
