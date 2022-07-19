# Flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wno-pointer-arith -O3 -fPIC ${CPPFLAGS} -fsanitize=undefined
LDFLAGS  = -shared

# Compiler and linker
CC = cc
