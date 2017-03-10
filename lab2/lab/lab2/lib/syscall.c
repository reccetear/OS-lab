#include "mystring.h"

int line = 0;
int row = 0;
/*
 * io lib here
 * 库函数写在这
 */

static inline int32_t syscall(int num, int check, uint32_t a1,uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
/*Generic system call: pass system call number in AX
up to five parameters in DX,CX,BX,DI,SI
Interrupt kernel with T_SYSCALL
	
The "volatile" tells the assembler not to optimize
this instruction away just because we don't use the
return value

The last clause tells the assembler that this can potentially
change the condition and arbitrary memory locations.*/

	/*Lab2 code here 嵌入汇编代码，调用int $0x80*/
	int32_t ret = 0;
	asm volatile("int $0x80": "=a"(ret) :"a"(num),"d"(check),"c"(a1), "b"(a2), "D"(a3), "S"(a4));
	return ret;
}


void printf(const char *format,...){
	void **args = (void **)&format + 1;
	int count = 0;
	int i = 0;
	for(i = 0; format[i] != '\0'; i++,row++) {
		if(row >= 80) {
			line += row / 80;
			row = row % 80;
		}
		if(strcmp((char *)&format[i], 2, "%s") == 0) {
			int len_temp = strlen(args[count]);
			syscall(4,1,(uint32_t)args[count],len_temp,line,row,0);
			row += len_temp - 1;
			count++;
			i++;
		}
		else if(strcmp((char *)&format[i], 2, "%d") == 0) {
			char num[80];
			i2str(num,(int)args[count]);
			int len_temp = strlen(num);
			syscall(4,1,(uint32_t)num,len_temp,line,row,0);
			row += len_temp - 1;
			count++;
			i++;
		}
		else if(strcmp((char *)&format[i], 2, "%x") == 0) {
			char num[80];
			x2str(num,(int)args[count]);
			int len_temp = strlen(num);
			syscall(4,1,(uint32_t)num,len_temp,line,row,0);
			row += len_temp - 1;
			count++;
			i++;
		}
		else if(strcmp((char *)&format[i], 2, "%c") == 0) {
			syscall(4,1,(uint32_t)&args[count],1,line,row,0);
			count++;
			i++;
		}
		else if(format[i] == '\n') {
			line++;
			row = -1;
		}
		else
			syscall(4,1,(uint32_t)&format[i],1,line,row,0);
	}
}
