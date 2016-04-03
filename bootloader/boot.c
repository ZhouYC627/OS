#include "boot.h"
#define SECTSIZE 512
#define L_A 0x8000
#define V_ADDR 0xB8000
void ramdisk_read(char *, int, int);
void print_s(char *s, int line){
	int i;
	for (i=0; s[i]!=0; i++){
		int *p = (void *)V_ADDR + (80*line+i)*2;
		*p = 0x0c00 | s[i];
	}
}

void bootmain(void)
{
	/* 这里是加载磁盘程序的代码 */
	struct ELFHeader *elf;
	struct ProgramHeader *ph;

	int i=0;
	char s[]="Loading";
	//char s1[]="...";
	// print "Loading..."
	//int s[4] = {0x64616f4c, 0x2e676e69, 0x00002e2e};
	print_s(s, 4);
	//print_s(s1, 3, 4);
	/*for (; i<10; i++){
		int *p = (void *)V_ADDR + (80*4+i)*2;
		*p = 0x0c00 | s[i];
	}
	*/
	//while(1);
	elf = (void *)L_A;
	ramdisk_read((void *)elf, 0, 4096);
	
	//ph=(void*)elf + elf->phoff;
	
	for (i=0; i < elf->phnum; i++){
		/* Scan the program header table, load each segment into memory */
		ph = (void*)elf + elf->phoff+(elf->phentsize) * i;
		//paddr = (void*)ph->paddr;
		/* read the content of the segment from the ELF file to the memory region ( 0x8000 ) */
		ramdisk_read((void *)ph->paddr, ph->off, ph->filesz);
		/* zero the memory region */
		//memset((void *)paddr + ph->filesz, 0, ph->memsz - ph->filesz);
		/*int j;
		for (j = ph->filesz; j< ph->memsz; j++){
			*(paddr+j) = 0;
		}*/
		//ph+=elf->phentsize;
	}
	//print "Executing..."
	//int s1[4] = { 0x63657845, 0x6e697475, 0x2e2e2e67};
	//s[0] = 0x63657845;
	//s[1] = 0x00657475;
	char s2[]="Back to boot.";
	print_s(s2, 11);
	/*s[0] = 0x6b636142;
	s[1] = 0x206f7420;
	s[2] = 0x746f6f42;
	print_s(s, 11);*/
	((void(*)(void))elf->entry)();
	while(1);
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* 等待磁盘完毕 */
}

/* 读磁盘的一个扇区 */
void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

void ramdisk_read(char *buf, int offset, int len){
	int se = offset / SECTSIZE + 1;
	char *temp = buf + offset;
	buf -= offset % SECTSIZE;
	for (; buf <=temp; buf += SECTSIZE, se++){
		readsect(buf , se);
	}
}

