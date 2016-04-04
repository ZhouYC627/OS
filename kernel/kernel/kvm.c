#include "x86.h"
#include "device.h"

#define SECTSIZE 512

SegDesc gdt[NR_SEGMENTS];       // the new GDT
TSS tss;

void
init_seg() { // setup kernel segements
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
    gdt[SEG_TSS].s = 0;
	set_gdt(gdt, sizeof(gdt));

	/*
	 * 初始化TSS
	 */
	tss.ss0 = KSEL(SEG_KDATA);
	int esp = 0;
	asm volatile("movl %%esp, %0" : "=r"(esp));
	tss.esp0 = esp;
	//asm volatile("ltr %0" : : "r"(USEL(SEG_TSS)));

	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));

	ltr(USEL(SEG_TSS));
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

}

void read_seg(unsigned char *buf, int offset, int len);
void
load_umain(void) {
    /*
     * Load your app here
     * 加载用户程序
     */
     struct ELFHeader *elf;
     struct ProgramHeader *ph, *eph;
     unsigned char* pa, *i;
     
     elf = (struct ELFHeader*)0x8000;
     read_seg((unsigned char*)elf, 200*SECTSIZE, 4096);
     
     ph = (struct ProgramHeader*)((char *)elf + elf->phoff);
     eph = ph + elf->phnum;

     for (; ph<eph; ph++){
	     //ph = (void*)(elf + elf->phoff + elf->phentsize);
	     pa = (unsigned char*)(ph->paddr) + 0x200000;
	     read_seg(pa, ph->off, ph->filesz);
	     for (i=pa+ph->filesz; i<pa+ph->memsz; *i ++ = 0);
     }
     //((void(*)(void))elf->entry)();
     gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0x200000, 0xffffffff, DPL_USER);
     gdt[SEG_UDATA] = SEG(STA_W,         0x200000, 0xffffffff, DPL_USER);
     eip = elf->entry;

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

void read_seg(unsigned char *buf, int offset, int len){
	int se = offset / SECTSIZE + 1;
	unsigned char *temp = buf + offset;
	buf -= offset % SECTSIZE;
	for (; buf <=temp; buf += SECTSIZE, se++){
		readsect(buf , se);
	}
}
