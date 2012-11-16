
CC=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-gcc -g -Os -msoft-float -fno-builtin -nostdinc -marm -mno-thumb-interwork -mabi=aapcs-linux -march=armv4 -Wall -fno-stack-protector -fstack-usage -fno-toplevel-reorder -c
LD=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-ld.bfd -S
OBJCOPY=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-objcopy
OBJDUMP=/opt/arm-cross-toolchain/bin/arm-linux-gnueabi-objdump

TARGET=led

all:
	${CC} -o ${TARGET}.o ${TARGET}.s
	${LD} -o ${TARGET} ${TARGET}.o -TLed.lds
	${OBJCOPY} -O binary ${TARGET} ${TARGET}.bin
	${OBJDUMP} -D ${TARGET} > objdump-asm

clean:
	@rm -f ${TARGET} ${TARGET}.o ${TARGET}.bin objdump-asm
