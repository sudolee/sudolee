# dt = download tool for qt2440

CC=arm-linux-gnueabi-gcc -g -Os -fno-builtin -nostdinc -marm -march=armv4 -msoft-float -mno-thumb-interwork -mabi=aapcs-linux -fno-stack-protector -Wall -c -I./include/ -I.
LD=arm-linux-gnueabi-ld -S
OBJCOPY=arm-linux-gnueabi-objcopy
OBJDUMP=arm-linux-gnueabi-objdump

TARGET=dt

all:
	${CC} -o start.o start.s
	${CC} -o led.o led.s
	${CC} -o main.o main.c
	${CC} -o clk_pwr.o clk_pwr.c
	${CC} -o memory.o memory.c
	${CC} -o irq.o irq.c
	${CC} -o uart.o uart.c
#	${CC} -o lib/list.o lib/list.c
#	${CC} -o lib/string.o lib/string.c
	${LD} -o ${TARGET} -TLink.lds \
		start.o led.o main.o clk_pwr.o memory.o irq.o
	${OBJCOPY} -O binary ${TARGET} ${TARGET}.bin
	${OBJDUMP} -D ${TARGET} > objdump-D-asm

clean:
	@rm -f *.o lib/*.o *.su objdump-D-asm ${TARGET}.bin ${TARGET} 
