PRJ=ipk-project_1

NAME=ipkcpc
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -lm -fcommon

run: ipkcpc.c
	${CC} ${CFLAGS} ipkcpc.c -o ${NAME} 

.PHONY: clean
clean:
	rm -f ipkcpc
	rm -f *.txt
