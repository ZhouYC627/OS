# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
LD = ld
CFLAGS = -m32 -march=i386 -static -MD -std=gnu89 -ggdb \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
		 -Wall -Werror -O2 -I./include -I./lib
ASFLAGS = -ggdb -m32 -MD
LDFLAGS = -melf_i386
QEMU = qemu-system-i386

define git_commit
	-@git add . -A --ignore-errors
	-@(echo "> play" && echo "141220162 Zhou YiChen" && uname -a && uptime && echo) | git commit -F - -q --no-verify --allow-empty
endef

os.img:
	@cd bootloader; make
	@cd kernel; make
	@cd app; make
	cat bootloader/bootblock kernel/kmain app/umain > os.img

play: os.img
 #	$(call git_commit)
	$(QEMU) -serial stdio os.img
	make clean

debug: os.img
	$(QEMU) -serial stdio -s -S os.img

clean:
	@cd bootloader; make clean
	@cd kernel; make clean
	@cd app; make clean
	rm -f os.img 

count:
	$(call git_commit)
	git checkout b5e3d79
	find . -name '*.[ch]' | xargs cat | wc -l
	git checkout master
	find . -name '*.[ch]' | xargs cat | wc -l

