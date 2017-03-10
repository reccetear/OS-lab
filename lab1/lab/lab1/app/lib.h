void print(char *str, int line, int row)
{
	asm volatile("movw $0x18, %ax	\n\t"
		     "movw %ax, %gs	\n\t");	

	asm volatile("movb $0x0d, %%ch	\n\t"
		     "movl %0, %%edi" : : "g"((80 * line + row) * 2) : "%cx", "%edi");

	int i;
	for(i = 0; str[i] != '\0'; i++)
	{
		asm volatile("movb %0, %%cl		\n\t"
			     "movw %%cx, %%gs:(%%edi)	\n\t"
			     "addl $0x2, %%edi"
				 : : "g"(str[i]));
	}
}
