.POSIX:

include config.mk

include files.mk

OBJ = ${SRC:.c=.o}

libah4.so: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

.c.o:
	${CC} -o $@ -c ${CFLAGS} $<

${OBJ}: config.mk Makefile

include depends.mk

clean:
	rm -f libah4.so ${OBJ}

.PHONY: clean
