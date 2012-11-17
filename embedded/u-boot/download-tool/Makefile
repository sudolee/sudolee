# dt = download tool for qt2440

#CC=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-gcc -g -Os -msoft-float -fno-builtin -nostdinc -marm -mno-thumb-interwork -mabi=aapcs-linux -march=armv4 -Wall -fno-stack-protector -fstack-usage -fno-toplevel-reorder -c
#LD=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-ld.bfd -S
#OBJCOPY=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-objcopy
#OBJDUMP=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-objdump

CC=arm-linux-gnueabi-gcc -g -Os -msoft-float -fno-builtin -nostdinc -marm -mno-thumb-interwork -mabi=aapcs-linux -march=armv4 -Wall -fno-stack-protector -fno-toplevel-reorder -c
LD=arm-linux-gnueabi-ld -S
OBJCOPY=arm-linux-gnueabi-objcopy
OBJDUMP=arm-linux-gnueabi-objdump

TARGET=dt

all:
	${CC} -o start.o start.s
	${CC} -o main.o main.c
	${CC} -o cpm.o cpm.c
	${CC} -o mem.o mem.c
	${LD} -o ${TARGET} -TLink.lds \
		start.o main.o cpm.o mem.o
	${OBJCOPY} -O binary ${TARGET} ${TARGET}.bin
	${OBJDUMP} -D ${TARGET} > objdump-D-asm

clean:
	@rm -fv *.o *.su objdump-D-asm ${TARGET}.bin ${TARGET} 
