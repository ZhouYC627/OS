# 系统启动时，BIOS会完成初始化，然后将磁盘第一个512字节保存到物理内存0x00007C00
# 位置，然后设置%CS=0000h, %IP=7C00h

#include "asm.h"
# 从此开始是16位代码

#define KOFFSET 0x0
.code16
.globl start
start:
    #real mode code here
    lgdt	gdtdesc
    movw	$0x2401, %ax
    int		$0x15
    cli
    movl	%cr0, %eax
    orl		$0x1, %eax
    movl	%eax, %cr0
    ljmp	$GDT_ENTRY(1), $start32
loop:
    jmp loop


.code32
start32:
    #protection mode code here
    movw	$GDT_ENTRY(2), %ax
    movw	%ax, %ds
    movw	%ax, %es
    movw	%ax, %ss
    movw	$GDT_ENTRY(3), %ax
    movw	%ax, %gs
    
    movl	$0x8000, %esp

    
    #Set up a stack for C code
    call	bootmain
    #jmp		loop32
    ;movl	$((80 * 5 + 0) * 2), %edi
    ;movb	$0x0c, %ah
    ;movb	$72, %al
    ;movw	%ax, %gs:(%edi)
    

#loop32:
#   jmp loop32


# GDT
.p2align 2                         # 对齐
    #GDT definition here
gdt:    #NULL
    .word    0,0
    .byte    0,0,0,0
    SEG_ASM(0xA, 0x0, 0xffffffff)
    SEG_ASM(0x2, 0x0, 0xffffffff)
    SEG_ASM(0x2, 0xB8000, 0xffffffff)

gdtdesc:                           # GDT描述符
	.word   (gdtdesc - gdt - 1)    # GDT长度，留意地址运算
	.long   gdt                    # GDT地址

