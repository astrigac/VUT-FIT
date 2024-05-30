PRJ=ios-project_2

NAME=proj2
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

run: proj2.c
	${CC} ${CFLAGS} proj2.c -o ${NAME} -pthread

.PHONY: clean
clean:
	rm -f ${NAME}