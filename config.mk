# Libraries
LIBS = -lpthread -lcjson -lGLEW -lSDL2

# Flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
#CFLAGS   = -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -Og -fPIC ${CPPFLAGS} -fsanitize=undefined
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O3 -fPIC -flto=auto ${CPPFLAGS}
LDFLAGS  = -shared -flto=auto ${LIBS}

# Compiler and linker
CC = cc
