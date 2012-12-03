# dt = download tool for qt2440

CC = arm-linux-gnueabi-gcc -g -Os -mabi=aapcs -mno-thumb-interwork -mfloat-abi=soft -mlittle-endian -mcpu=arm920t -marm -fno-builtin -nostdinc -fno-common -fstack-usage -Wall -I./include/ -I. -c
LD=arm-linux-gnueabi-ld -S
OBJCOPY=arm-linux-gnueabi-objcopy
OBJDUMP=arm-linux-gnueabi-objdump

TARGET=dt

all:
	${CC} -o start.o start.s
	${CC} -o led.o led.s
	${CC} -o board.o board.c
	${CC} -o clk_pwr.o clk_pwr.c
	${CC} -o memory.o memory.c
	${CC} -o irq.o irq.c
	${CC} -o uart.o uart.c
	${CC} -o timer.o timer.c
#	${CC} -o nand.o nand.c
	${CC} -o nand-tiny.o nand-tiny.c
	${CC} -o lib/list.o lib/list.c
	${CC} -o lib/string.o lib/string.c
	${CC} -o lib/printf.o lib/printf.c
	${CC} -o lib/lshrdi3.o lib/lshrdi3.s
	${CC} -o dt-main.o dt-main.c
	${LD} -o ${TARGET} -TLink.lds \
		start.o board.o clk_pwr.o memory.o uart.o lib/printf.o lib/lshrdi3.o nand-tiny.o dt-main.o
#		start.o led.o board.o clk_pwr.o memory.o uart.o lib/printf.o nand.o
	${OBJCOPY} -O binary ${TARGET} ${TARGET}.bin
	${OBJDUMP} -D ${TARGET} > objdump-D-asm
	gcc usr/mkimage.c -o usr/mkimage

clean:
	@rm -f *.o lib/*.o *.su lib/*.su objdump-D-asm ${TARGET}.bin ${TARGET} 
	@rm -f usr/mkimage
