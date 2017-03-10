#ifndef __lib_h__
#define __lib_h__

void printf(const char *format,...);
int fork(void);
void sleep(unsigned int time);
void exit(int status);
void createSem(void);
void unlocksem(void);
void locksem(void);
void destroySem(void);

#endif
