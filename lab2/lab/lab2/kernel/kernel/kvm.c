#include "x86.h"
#include "device.h"

#define SECTSIZE 512
#define ELF_OFFSET_DISK 200 * SECTSIZE
#define STACK 0x800000
#define USER_CS_BASE 0x200000
#define USER_SS_BASE 0x200000

void readdisk(unsigned char *elf, int count, int offset);
SegDesc gdt[NR_SEGMENTS];       // the new GDT
TSS tss;
struct ELFHeader *elf;
uint8_t buf[4096];

void
init_seg() { 
	// setup kernel segements
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, USER_CS_BASE,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         USER_SS_BASE,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
    	gdt[SEG_TSS].s = 0;
	set_gdt(gdt, sizeof(gdt));

    /*
	 * 初始化TSS
	 */
	tss.esp0 = STACK;
	tss.ss0 = KSEL(SEG_KDATA);
	ltr(KSEL(SEG_TSS));
	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));

	lldt(0);
}

void
enter_user_space(void) {
    /*
     * Before enter user space 
     * you should set the right segment registers here
     * and use 'iret' to jump to ring3
     * 进入用户空间
     */
	asm volatile("movw %%ax,%%es":: "a" (USEL(SEG_UDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (USEL(SEG_UDATA)));

	asm volatile("pushl %0		\n\t"
		     "pushl %1		\n\t"
		     "pushl $0x2	\n\t"
		     "pushl %2		\n\t"
		     "pushl %3		\n\t"
		     "iret		\n\t"
			::"i"(USEL(SEG_UDATA)),
			  "i"(STACK),
			  "i"(USEL(SEG_UCODE)),
			  "g"(elf->entry));
}

void
load_umain(void) {
    /*
     * Load your app here
     * 加载用户程序
     */
	struct ProgramHeader *ph, *eph;
	unsigned char * pa, *i;
	elf = (void *)buf;
	readdisk((void *)elf, 4096, ELF_OFFSET_DISK);
	
	ph = (struct ProgramHeader *)((char *)elf + elf->phoff);
	eph = ph + elf->phnum;
	for(;ph < eph; ph++)
	{
		if(ph->type == 1)
		{
			pa = (unsigned char *)ph->paddr + USER_CS_BASE;
			readdisk(pa, ph->filesz, ph->off + ELF_OFFSET_DISK);
			for(i = pa + ph->filesz; i < pa + ph->memsz; *i++ = 0);
		}
	}
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); 
}

void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte2(0x1F2, 1);
	out_byte2(0x1F3, offset);
	out_byte2(0x1F4, offset >> 8);
	out_byte2(0x1F5, offset >> 16);
	out_byte2(0x1F6, (offset >> 24) | 0xE0);
	out_byte2(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}


void readdisk(unsigned char *elf, int count, int offset) {
	unsigned char * eelf;
	eelf = elf + count;
	elf -= offset % SECTSIZE;
	offset = offset / SECTSIZE + 1;
	for(; elf < eelf; elf += SECTSIZE, offset++)
		readsect(elf,offset);
}
