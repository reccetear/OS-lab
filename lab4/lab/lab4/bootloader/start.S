# 系统启动时，BIOS会完成初始化，然后将磁盘第一个512字节保存到物理内存0x00007C00
# 位置，然后设置%CS=0000h, %IP=7C00h

#include "asm.h"
# 从此开始是16位代码
.code16
.globl start
start:
	cli

	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	movw $0x2401, %ax
	int $0x15

	lgdt gdtdesc

	movl %cr0, %eax
	orl $0x1, %eax
	movl %eax, %cr0
	
	ljmp $GDT_ENTRY(1), $start32
.code32
start32:
	movw $GDT_ENTRY(2), %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	movl $0x8000, %esp

	call bootmain

# GDT
.p2align 2                 # 对齐
gdt:   
    	SEG_NULLASM
	SEG_ASM(0xA, 0x0, 0xffffffff, 0x0)
	SEG_ASM(0x2, 0x0, 0xffffffff, 0x0)

gdtdesc:                           # GDT描述符
	.word   (gdtdesc - gdt - 1)    # GDT长度，留意地址运算
	.long   gdt                    # GDT地址
