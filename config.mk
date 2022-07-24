# Libraries
LIBS = -lpthread -lcjson -lGLEW

# Flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
#CFLAGS   = -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O0 -fPIC ${CPPFLAGS} -fsanitize=undefined
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O3 -fPIC ${CPPFLAGS}
LDFLAGS  = -shared ${LIBS}

# Compiler and linker
CC = cc
