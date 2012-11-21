# dt = download tool for qt2440

CC=arm-linux-gnueabi-gcc -g -Os -msoft-float -fno-builtin -nostdinc -marm -mno-thumb-interwork -mabi=aapcs-linux -fno-stack-protector -march=armv4 -Wall -c
LD=arm-linux-gnueabi-ld -S
OBJCOPY=arm-linux-gnueabi-objcopy
OBJDUMP=arm-linux-gnueabi-objdump

TARGET=dt

all:
	${CC} -o start.o start.s
	${CC} -o led.o led.s
	${CC} -o main.o main.c
	${CC} -o cpm.o cpm.c
	${CC} -o mem.o mem.c
	${CC} -o irq.o irq.c
#	${CC} -o list.o list.c
	${LD} -o ${TARGET} -TLink.lds \
		start.o led.o main.o cpm.o mem.o irq.o
	${OBJCOPY} -O binary ${TARGET} ${TARGET}.bin
	${OBJDUMP} -D ${TARGET} > objdump-D-asm

clean:
	@rm -f *.o *.su objdump-D-asm ${TARGET}.bin ${TARGET} 
