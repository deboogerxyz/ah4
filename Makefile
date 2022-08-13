include config.mk

CSRC   = $(shell find -name "*.c")
CXXSRC = $(shell find -name "*.cpp")

OBJ = ${CSRC:.c=.o} ${CXXSRC:.cpp=.o}
DEP = ${OBJ:.o=.d}

libah4.so: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

-include ${DEP}

.c.o:
	${CC} -MM -MT $@ -MF $(patsubst %.o, %.d, $@) ${CFLAGS} $<
	${CC} -o $@ -c ${CFLAGS} $<

.cpp.o:
	${CXX} -MM -MT $@ -MF $(patsubst %.o, %.d, $@) ${CXXFLAGS} $<
	${CXX} -o $@ -c ${CXXFLAGS} $<

${OBJ}: config.mk Makefile

clean:
	rm -f libah4.so ${OBJ} ${DEP}

.PHONY: clean
