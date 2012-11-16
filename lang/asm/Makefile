OBJ=inline-asm

all:
	gcc -fno-builtin -S ${OBJ}.c -o ${OBJ}.s
	as  ${OBJ}.s -o ${OBJ}.o
	ld -e mymain ${OBJ}.o -o ${OBJ}

clean:
	@rm -f ${OBJ}.s ${OBJ}.o ${OBJ}
